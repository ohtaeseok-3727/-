#include "stdafx.h"
#include "FrameProfiler.h"
#include "Dependencies/glew.h"
#include <Windows.h>
#include <algorithm>
#include <iomanip>
#include <iostream>
#include <numeric>
#include <sstream>

FrameProfiler::~FrameProfiler()
{
    if (m_IntervalFrames)
    {
        WriteInterval(Clock::now());
    }
}

void FrameProfiler::OpenLog()
{
    CreateDirectoryW(L"runtime-logs", nullptr);
    const std::wstring directory = L"runtime-logs\\performance";
    CreateDirectoryW(directory.c_str(), nullptr);
    SYSTEMTIME time{};
    GetSystemTime(&time);
    std::wostringstream path;
    path << directory << L"\\session-" << time.wYear << L'-' << time.wMonth << L'-' << time.wDay
         << L'-' << time.wHour << L'-' << time.wMinute << L'-' << time.wSecond << L'-'
         << GetCurrentProcessId() << L".csv";
    m_Log.open(path.str().c_str(), std::ios::out);
    if (!m_Log)
    {
        std::cerr << "성능 로그 파일 생성 실패: 콘솔 집계만 계속합니다.\n";
        return;
    }
    std::ofstream metadata((path.str() + L".txt").c_str());
    if (metadata)
    {
        metadata << "schema=2\nrevision=relative-performance-log-v2\nbuilt=" << __DATE__ << ' '
                 << __TIME__;
#ifdef _DEBUG
        metadata << "\nconfiguration=Debug";
#else
        metadata << "\nconfiguration=Release";
#endif
        for (GLenum name : {GL_VENDOR, GL_RENDERER, GL_VERSION})
        {
            const auto value = glGetString(name);
            metadata << '\n'
                     << name << '=' << (value ? reinterpret_cast<const char*>(value) : "unknown");
        }
        metadata << "\ntiming=CPU wall time, not GPU time\n";
    }
    m_Log
        << "schema,utc,row,window_s,frames,fps,cadence_mean_ms,cadence_p95_ms,cadence_max_ms,"
           "over_33ms,over_50ms,active_mean_ms,world_cpu_ms,post_cpu_ms,ui_cpu_ms,present_cpu_ms,"
           "draw_mean,world_draws,post_draws,ui_draws,present_draws,batches,vertices,upload_bytes,"
           "sprite_requests,sprite_batch_reuses,text_hits,text_misses,geometry_hits,geometry_misses,"
           "updates,update_mean_ms,width,height,effect_flags,focused,paused\n";
    std::cout << "성능 CSV 저장: runtime-logs\\performance (작업 디렉터리 기준, 실행별 파일)\n";
}

void FrameProfiler::BeginFrame()
{
    if (!m_Started)
    {
        OpenLog();
        m_FrameIntervals.reserve(1024);
        m_IntervalStart = Clock::now();
        m_Started = true;
    }
    const auto now = Clock::now();
    if (m_PreviousStart != Clock::time_point{})
    {
        m_FrameIntervals.push_back(
            std::chrono::duration<double, std::milli>(now - m_PreviousStart).count());
    }
    m_PreviousStart = m_FrameStart = m_PhaseStart = now;
    m_Phase = Phase::World;
    m_CurrentDrawCalls = 0;
    m_FrameActive = true;
}

void FrameProfiler::SetPhase(Phase phase)
{
    if (!m_FrameActive)
    {
        return;
    }
    const auto now = Clock::now();
    m_PhaseMs[static_cast<std::size_t>(m_Phase)] +=
        std::chrono::duration<double, std::milli>(now - m_PhaseStart).count();
    m_PhaseStart = now;
    m_Phase = phase;
}

void FrameProfiler::RecordDrawCall()
{
    if (m_FrameActive)
    {
        ++m_CurrentDrawCalls;
        ++m_PhaseDraws[static_cast<std::size_t>(m_Phase)];
    }
}

void FrameProfiler::RecordBatch(std::size_t vertices, std::size_t bytes)
{
    if (m_FrameActive)
    {
        ++m_Batches;
        m_Vertices += vertices;
        m_Bytes += bytes;
    }
}

void FrameProfiler::RecordSprite(bool reused)
{
    if (m_FrameActive)
    {
        ++m_Sprites;
        m_Reused += reused ? 1 : 0;
    }
}

void FrameProfiler::RecordText(bool cached)
{
    if (m_FrameActive)
    {
        cached ? ++m_TextHits : ++m_TextMisses;
    }
}

void FrameProfiler::EndFrame()
{
    if (!m_FrameActive)
    {
        return;
    }
    SetPhase(m_Phase);
    const auto now = Clock::now();
    m_ActiveMs += std::chrono::duration<double, std::milli>(now - m_FrameStart).count();
    m_FrameActive = false;
    ++m_IntervalFrames;
    m_IntervalDrawCalls += m_CurrentDrawCalls;
    if (std::chrono::duration<double>(now - m_IntervalStart).count() >= 1.0)
    {
        WriteInterval(now);
    }
}

void FrameProfiler::RecordGeometry(bool cached)
{
    if (m_FrameActive)
    {
        cached ? ++m_GeometryHits : ++m_GeometryMisses;
    }
}

void FrameProfiler::RecordUpdate(double milliseconds)
{
    m_UpdateMs += milliseconds;
    ++m_Updates;
}

void FrameProfiler::SetContext(int width, int height, unsigned effects, bool focused, bool paused)
{
    m_Width = width;
    m_Height = height;
    m_Effects = effects;
    m_Focused = focused;
    m_Paused = paused;
}

void FrameProfiler::WriteInterval(Clock::time_point now)
{
    const double seconds = std::chrono::duration<double>(now - m_IntervalStart).count();
    const double frames = static_cast<double>(m_IntervalFrames);
    std::sort(m_FrameIntervals.begin(), m_FrameIntervals.end());
    const auto count = m_FrameIntervals.size();
    const double mean =
        count ? std::accumulate(m_FrameIntervals.begin(), m_FrameIntervals.end(), 0.0) / count : 0;
    const double p95 = count ? m_FrameIntervals[(count * 95 + 99) / 100 - 1] : 0;
    const double maximum = count ? m_FrameIntervals.back() : 0;
    const auto slow = [&](double limit)
    {
        return m_FrameIntervals.end() -
               std::upper_bound(m_FrameIntervals.begin(), m_FrameIntervals.end(), limit);
    };
    const double fps = seconds > 0 ? frames / seconds : 0;
    std::ostringstream message;
    message << std::fixed << std::setprecision(1) << "성능: 최근 " << m_IntervalFrames
            << "프레임, FPS " << fps << ", glDraw/프레임 " << m_IntervalDrawCalls / frames
            << " (최신 " << m_CurrentDrawCalls << "회)"
            << ", 프레임 간격 p95/최대 " << p95 << '/' << maximum << "ms\n";
    std::cout << message.str();
    // Limit each session to approximately 24 hours of interval rows.
    if (m_Log.is_open() && m_Rows < 86400)
    {
        SYSTEMTIME utc{};
        GetSystemTime(&utc);
        m_Log << std::fixed << std::setprecision(3) << "2," << utc.wYear << '-' << utc.wMonth << '-'
              << utc.wDay << 'T' << utc.wHour << ':' << utc.wMinute << ':' << utc.wSecond << "Z,"
              << ++m_Rows << ',' << seconds << ',' << m_IntervalFrames << ',' << fps << ',' << mean
              << ',' << p95 << ',' << maximum << ',' << slow(33.0) << ',' << slow(50.0) << ','
              << m_ActiveMs / frames;
        for (double value : m_PhaseMs)
        {
            m_Log << ',' << value / frames;
        }
        m_Log << ',' << m_IntervalDrawCalls / frames;
        for (auto value : m_PhaseDraws)
        {
            m_Log << ',' << value;
        }
        m_Log << ',' << m_Batches << ',' << m_Vertices << ',' << m_Bytes << ',' << m_Sprites << ','
              << m_Reused << ',' << m_TextHits << ',' << m_TextMisses << ',' << m_GeometryHits
              << ',' << m_GeometryMisses << ',' << m_Updates << ','
              << (m_Updates ? m_UpdateMs / m_Updates : 0.0) << ',' << m_Width << ',' << m_Height
              << ',' << m_Effects << ',' << m_Focused << ',' << m_Paused << '\n';
        m_Log.flush();
        if (!m_Log || m_Rows == 86400)
        {
            std::cerr << "성능 CSV 기록 중단: 쓰기 실패 또는 실행별 86400행 한도 도달.\n";
            m_Log.close();
        }
    }
    m_IntervalStart = now;
    m_IntervalFrames = m_IntervalDrawCalls = 0;
    m_Batches = m_Vertices = m_Bytes = m_Sprites = m_Reused = m_TextHits = m_TextMisses = 0;
    m_ActiveMs = 0;
    m_UpdateMs = 0;
    m_Updates = m_GeometryHits = m_GeometryMisses = 0;
    m_PhaseMs.fill(0);
    m_PhaseDraws.fill(0);
    m_FrameIntervals.clear();
}
