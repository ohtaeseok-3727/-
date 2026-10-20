#pragma once
#include <array>

// User balance (2026-10-08); geometry/timing follows the existing preview.
namespace KnightSkillData
{
    inline constexpr int AuraLevel = 5;
    inline constexpr float AttackMultiplier = 1.20f;
    inline constexpr float RangeMultiplier = 1.30f;
    inline constexpr float PlayerScale = .45f * .32f;
    inline constexpr float PreviewScale = PlayerScale / .61f;
    inline constexpr float ThrustDuration = .610f;
    inline constexpr float ThrustEffectDuration = .145f;
    inline constexpr float WaveRelease = .360f;
    inline constexpr float WaveCastDuration = .680f;
    inline constexpr float WaveFlight = .420f;
    inline constexpr float WaveDistance = 840.f * .51f;

    // Cheondan timing is presentation tuning; user dimensions are relative to Wave.
    inline constexpr float UltimateCharge = .650f;
    inline constexpr float UltimateSwingEnd = .820f;
    inline constexpr float UltimateRelease = 1.050f;
    inline constexpr float UltimateCastDuration = 1.250f;
    inline constexpr float UltimateHeight = 5.f;
    inline constexpr float UltimateDistance = 2.f;
    inline constexpr float UltimateFlight = WaveFlight * UltimateDistance;

    struct StrikeEvent
    {
        float time, x, y;
    };

    inline constexpr std::array<StrikeEvent, 6> Strikes{{{.180f, 43, 17},
                                                         {.210f, -32, -57},
                                                         {.240f, 12, 68},
                                                         {.270f, -8, -6},
                                                         {.300f, -23, 44},
                                                         {.330f, 24, -34}}};

    // Bounds enclose pixels with alpha >= 32, excluding transparent cell margins.
    struct Frame
    {
        int x, y, w, h;
        float pivotX, pivotY;
        float left, top, right, bottom;
    };

    inline constexpr std::array<Frame, 4> ThrustBody{{{0, 0, 543, 724, 225, 588},
                                                      {543, 0, 543, 724, 240, 588},
                                                      {1086, 0, 580, 724, 214, 588},
                                                      {1629, 0, 543, 724, 242, 588}}};
    inline constexpr std::array<Frame, 4> ThrustEffect{
        {{0, 0, 768, 512, 175, 310, 121, 226, 396, 391},
         {768, 0, 768, 512, 133, 310, 83, 226, 580, 391},
         {0, 512, 768, 512, 171, 225, 98, 127, 746, 321},
         {768, 512, 768, 512, 130, 226, 83, 150, 674, 300}}};
    inline constexpr std::array<Frame, 4> WaveBody{{{0, 0, 384, 512, 188, 453},
                                                    {384, 0, 384, 512, 190, 453},
                                                    {768, 0, 444, 512, 180, 453},
                                                    {1232, 0, 304, 512, 106, 453}}};
    inline constexpr std::array<Frame, 4> WaveEffect{
        {{0, 512, 384, 512, 230, 232, 158, 144, 280, 295},
         {384, 512, 384, 512, 219, 234, 81, 98, 297, 335},
         {768, 512, 384, 512, 241, 228, 42, 67, 338, 375},
         {1152, 512, 384, 512, 226, 225, 89, 106, 322, 343}}};

    inline int ThrustPhase(float age)
    {
        return age < .030f ? 0 : age < .065f ? 1 : age < .110f ? 2 : 3;
    }

    inline int WavePhase(float age)
    {
        return age < .045f ? 0 : age < .090f ? 1 : age < .310f ? 2 : 3;
    }
}
