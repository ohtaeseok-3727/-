#pragma once
#include <chrono>
#include <cstdint>
#include <array>
#include <fstream>
#include <vector>

// Measures completed presentation frames and their actual OpenGL draw submissions.
class FrameProfiler
{
  public:
    enum class Phase
    {
        World,
        Post,
        Ui,
        Present
    };
    ~FrameProfiler();
    void SetPhase(Phase phase);
    void RecordBatch(std::size_t vertices, std::size_t bytes);
    void RecordSprite(bool reused);
    void RecordText(bool cached);
    void RecordGeometry(bool cached);
    void RecordUpdate(double milliseconds);
    void SetContext(int width, int height, unsigned effects, bool focused, bool paused);
    void BeginFrame();
    void RecordDrawCall();
    void EndFrame();

  private:
    using Clock = std::chrono::steady_clock;
    void OpenLog();
    void WriteInterval(Clock::time_point now);
    Clock::time_point m_FrameStart{}, m_PreviousStart{}, m_PhaseStart{};
    std::array<double, 4> m_PhaseMs{};
    std::array<std::uint64_t, 4> m_PhaseDraws{};
    std::vector<double> m_FrameIntervals;
    std::ofstream m_Log;
    Phase m_Phase = Phase::World;
    std::uint64_t m_Batches = 0, m_Vertices = 0, m_Bytes = 0;
    std::uint64_t m_Sprites = 0, m_Reused = 0, m_TextHits = 0, m_TextMisses = 0;
    std::uint64_t m_Rows = 0;
    double m_ActiveMs = 0;
    double m_UpdateMs = 0;
    std::uint64_t m_Updates = 0, m_GeometryHits = 0, m_GeometryMisses = 0;
    int m_Width = 0, m_Height = 0;
    unsigned m_Effects = 0;
    bool m_Focused = true, m_Paused = false;

    Clock::time_point m_IntervalStart = {};
    std::uint64_t m_CurrentDrawCalls = 0;
    std::uint64_t m_IntervalDrawCalls = 0;
    std::uint64_t m_IntervalFrames = 0;
    bool m_Started = false;
    bool m_FrameActive = false;
};
