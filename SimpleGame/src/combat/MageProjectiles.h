#pragma once
#include "LevelMap.h"
#include <cstdint>
#include <unordered_set>
#include <vector>

class GameSession;
struct SkillUseRequest;

enum class MageProjectileKind
{
    Wind,
    Sword
};

struct MageProjectile
{
    MageProjectileKind kind = MageProjectileKind::Wind;
    WorldPosition origin{}, position{};
    float age = 0.f, delay = 0.f, distance = 0.f;
    float speed = 0.f, range = 0.f;
    int variant = 0, damage = 0;
    bool faceLeft = false;
    std::unordered_set<std::uint64_t> hitTargets;
    bool Visible() const;
};

class MageProjectiles
{
  public:
    static constexpr float CharacterScale = .35f;
    // CombatVisualData's mage body already carries a .42 source-pixel scale.
    static constexpr float PlayerScale = CharacterScale * .42f;
    static constexpr float Height = LevelTuning::ProjectileHeight;
    void Reset();
    void Cast(const SkillUseRequest& request, WorldPosition player, bool faceLeft);
    void Update(float dt, GameSession& level, const LevelMap& map);
    bool Casting() const;
    float CastMilliseconds() const;
    bool CastFaceLeft() const;
    const std::vector<MageProjectile>& Projectiles() const;

  private:
    float m_CastTime = 1.f;
    bool m_FaceLeft = false;
    std::vector<MageProjectile> m_Projectiles;
};
