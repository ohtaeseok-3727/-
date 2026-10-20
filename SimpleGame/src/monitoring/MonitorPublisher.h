#pragma once

#include "MonitorSnapshot.h"
#include <chrono>
#include <condition_variable>
#include <mutex>
#include <thread>

// Only serialized, immutable snapshots cross into this worker.
class MonitorPublisher
{
  public:
    MonitorPublisher() = default;
    ~MonitorPublisher();
    MonitorPublisher(const MonitorPublisher&) = delete;
    MonitorPublisher& operator=(const MonitorPublisher&) = delete;
    bool Due();
    void Publish(const MonitorSnapshot& snapshot);

  private:
    void Run();
    static void Send(const std::string& message);
    std::chrono::steady_clock::time_point m_Next{};
    std::string m_Session;
    std::uint64_t m_Sequence = 0;
    std::mutex m_Mutex;
    std::condition_variable m_Wake;
    std::string m_Pending;
    bool m_Stopping = false;
    std::thread m_Worker;
};
