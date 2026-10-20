#pragma once
#include "LevelTypes.h"

class GameSession;
struct SkillUseRequest;

// A single circular channel: decorative swords never create extra damage events.
class MageUltimate
{
  public:
    static constexpr float Radius = 300.f;
    static constexpr int PulseCount = 40;
    static constexpr double RainDuration = 4.0;
    static constexpr double SummonDuration = .70;
    static constexpr double MergeStart = SummonDuration + RainDuration;
    static constexpr double ExplosionTime = MergeStart + .55;
    static constexpr double Duration = ExplosionTime + .55;

    void Reset();
    void Cast(const SkillUseRequest& request, WorldPosition player, bool faceLeft);
    void Update(float dt, GameSession& session);

    bool Active() const
    {
        return m_Time < Duration;
    }

    bool Casting() const
    {
        return m_Time < ExplosionTime;
    }

    float Time() const
    {
        return static_cast<float>(m_Time);
    }

    WorldPosition Center() const
    {
        return m_Center;
    }

    bool FaceLeft() const
    {
        return m_FaceLeft;
    }

    int PulsesApplied() const
    {
        return m_Pulses;
    }

    bool Exploded() const
    {
        return m_Exploded;
    }

    static double PulseTime(int index)
    {
        return SummonDuration + (index + 1) * RainDuration / PulseCount;
    }

  private:
    double m_Time = Duration;
    WorldPosition m_Center{};
    int m_Pulses = 0, m_Damage = 0, m_BurstDamage = 0;
    bool m_FaceLeft = false, m_Exploded = false;
};
