#include "stdafx.h"
#include "MonitorPublisher.h"
#include <windows.h>
#include <winhttp.h>
#include <memory>
#include <utility>

#pragma comment(lib, "winhttp.lib")

namespace
{
    struct CloseHttp
    {
        void operator()(void* handle) const
        {
            if (handle)
            {
                WinHttpCloseHandle(handle);
            }
        }
    };

    using HttpHandle = std::unique_ptr<void, CloseHttp>;
}

MonitorPublisher::~MonitorPublisher()
{
    {
        std::lock_guard<std::mutex> lock(m_Mutex);
        m_Stopping = true;
        m_Pending.clear();
    }
    m_Wake.notify_one();
    if (m_Worker.joinable())
    {
        m_Worker.join();
    }
}

bool MonitorPublisher::Due()
{
    const auto now = std::chrono::steady_clock::now();
    if (now < m_Next)
    {
        return false;
    }
    m_Next = now + std::chrono::seconds(1);
    return true;
}

void MonitorPublisher::Publish(const MonitorSnapshot& snapshot)
{
    if (m_Session.empty())
    {
        const auto stamp = std::chrono::system_clock::now().time_since_epoch().count();
        m_Session = std::to_string(GetCurrentProcessId()) + "-" + std::to_string(stamp);
    }
    auto message = MonitorJson::Serialize(snapshot, m_Session, ++m_Sequence);
    // Match the receiver's bounded message size. Do not send partial entity lists.
    if (message.size() > 2 * 1024 * 1024)
    {
        OutputDebugStringA("World monitor: snapshot exceeds 2 MiB; skipped.\n");
        return;
    }
    if (!m_Worker.joinable())
    {
        m_Worker = std::thread(&MonitorPublisher::Run, this);
    }
    {
        std::lock_guard<std::mutex> lock(m_Mutex);
        m_Pending = std::move(message);
    }
    m_Wake.notify_one();
}

void MonitorPublisher::Run()
{
    for (;;)
    {
        std::string message;
        {
            std::unique_lock<std::mutex> lock(m_Mutex);
            m_Wake.wait(lock,
                        [this]
                        {
                            return m_Stopping || !m_Pending.empty();
                        });
            if (m_Stopping)
            {
                return;
            }
            message.swap(m_Pending);
        }
        Send(message);
    }
}

void MonitorPublisher::Send(const std::string& message)
{
    HttpHandle session(WinHttpOpen(L"SimpleGameMonitor/1",
                                   WINHTTP_ACCESS_TYPE_NO_PROXY,
                                   WINHTTP_NO_PROXY_NAME,
                                   WINHTTP_NO_PROXY_BYPASS,
                                   0));
    if (!session || !WinHttpSetTimeouts(session.get(), 250, 250, 250, 250))
    {
        return;
    }
    HttpHandle connection(WinHttpConnect(session.get(), L"127.0.0.1", 8766, 0));
    if (!connection)
    {
        return;
    }
    HttpHandle request(WinHttpOpenRequest(connection.get(),
                                          L"POST",
                                          L"/api/snapshot",
                                          nullptr,
                                          WINHTTP_NO_REFERER,
                                          WINHTTP_DEFAULT_ACCEPT_TYPES,
                                          0));
    if (!request)
    {
        return;
    }
    DWORD disabled = WINHTTP_DISABLE_REDIRECTS | WINHTTP_DISABLE_COOKIES;
    if (!WinHttpSetOption(
            request.get(), WINHTTP_OPTION_DISABLE_FEATURE, &disabled, sizeof(disabled)))
    {
        return;
    }
    const auto length = static_cast<DWORD>(message.size());
    if (!WinHttpSendRequest(request.get(),
                            L"Content-Type: application/json; charset=utf-8\r\n",
                            static_cast<DWORD>(-1),
                            const_cast<char*>(message.data()),
                            length,
                            length,
                            0) ||
        !WinHttpReceiveResponse(request.get(), nullptr))
    {
        return;
    }
    DWORD status = 0;
    DWORD size = sizeof(status);
    if (WinHttpQueryHeaders(request.get(),
                            WINHTTP_QUERY_STATUS_CODE | WINHTTP_QUERY_FLAG_NUMBER,
                            WINHTTP_HEADER_NAME_BY_INDEX,
                            &status,
                            &size,
                            WINHTTP_NO_HEADER_INDEX) &&
        status != 200)
    {
        OutputDebugStringA("World monitor: receiver rejected a snapshot.\n");
    }
}
