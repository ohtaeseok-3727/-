#pragma once
#include "LevelTypes.h"
#include <cstdint>
#include <string>
#include <unordered_set>
#include <vector>
class GameSession;
class LevelMap;
struct SkillUseRequest;
enum class KnightSkillKind
{
    Thrust,
    Wave,
    Ultimate
};

struct KnightEffectBounds
{
    float left, top, right, bottom;
};

struct KnightSkillShot
{
    KnightSkillKind kind = KnightSkillKind::Thrust;
    WorldPosition caster{}, origin{}, position{};
    float age = 0, delay = 0, height = 0, scale = 1, distance = 0;
    int damage = 0;
    bool faceLeft = false, blocked = false;
    std::unordered_set<std::uint64_t> hitTargets;
    bool Visible() const;
    int Phase() const;
    float Duration() const;
    KnightEffectBounds Bounds() const;
};

class KnightSkills
{
  public:
    void Reset();
    void Cast(const SkillUseRequest& request,
              WorldPosition player,
              bool faceLeft,
              float rangeScale);
    void Update(float dt, GameSession& level, const LevelMap& map);
    bool Casting() const;

    float CastTime() const
    {
        return m_Time;
    }

    bool FaceLeft() const
    {
        return m_FaceLeft;
    }

    KnightSkillKind Kind() const
    {
        return m_Kind;
    }

    float UltimateVisualTime() const
    {
        return m_UltimateVisualTime;
    }

    bool UltimateVisualFaceLeft() const
    {
        return m_UltimateVisualFaceLeft;
    }

    const std::vector<KnightSkillShot>& Shots() const
    {
        return m_Shots;
    }

  private:
    void Step(float dt, GameSession& level, const LevelMap& map);
    float m_Time = 2;
    float m_UltimateVisualTime = 2;
    bool m_UltimateVisualFaceLeft = false;
    bool m_FaceLeft = false;
    KnightSkillKind m_Kind = KnightSkillKind::Thrust;
    std::vector<KnightSkillShot> m_Shots;
};
