#pragma once
#include "LevelMap.h"
#include "EnemyAIState.h"
#include "EnemyStateMachine.h"
#include <random>
#include <vector>
#include <cstdint>
#include <unordered_set>

enum class EnemyRole
{
    Melee,
    Ranged,
    Healer,
    Boss
};

enum class EnemyAction
{
    None,
    Swing,
    JumpSlam,
    BattleCry
};

// Authored by the chapter; a boss is not automatically immortal or a particular species.
struct EnemyStats
{
    int health = LevelTuning::EnemyHealth;
    int damage = LevelTuning::EnemyDamage;
    float speed = 68.f;
    float injuredSpeed = 68.f;
    float detectionRange = 430.f;
    int minimumHealth = 0;
    bool preserveCorpse = false;
    int appearance = 0; // Chapter-local visual/archetype ID.
    bool scriptedAI = false;
};

struct FieldEnemy
{
    std::uint64_t id = 0;
    EnemyAIState ai;
    WorldPosition position;
    int health = LevelTuning::EnemyHealth;
    int maxHealth = LevelTuning::EnemyHealth;
    float cooldown = 1.f;
    float windup = 0.f;
    float respawn = 0.f;
    float hitFlash = 0.f;
    std::size_t encounter = 0;
    EnemyRole role = EnemyRole::Melee;
    EnemyStats stats;
    float animationTime = 0.f;
    bool faceLeft = false;
    bool armorBroken = false;
    bool legInjured = false;
    float stateTime = 0.f;
    EnemyAction action = EnemyAction::None;
    float actionTime = 0.f;
    float actionDuration = 1.f;
    WorldPosition actionTarget{};
    WorldPosition actionOrigin{};
    float attackSpeedTime = 0.f;
};

class EnemySystem
{
  public:
    void Reset(unsigned seed, const LevelMap& map);
    void Clear();
    void Add(WorldPosition position,
             std::size_t encounter,
             EnemyRole role = EnemyRole::Melee,
             const EnemyStats& stats = {});
    bool Alive(std::size_t encounter) const;
    void RemoveEncounter(std::size_t encounter);
    FieldEnemy* Boss();
    bool DamageEnemy(std::uint64_t id, int damage);
    std::vector<WorldPosition> ConsumeScriptDefeats();
    void SetSupportEnabled(bool enabled);
    int Update(float dt, WorldPosition player, const LevelMap& map);
    std::vector<WorldPosition> Strike(WorldPosition player,
                                      bool faceLeft,
                                      int damage,
                                      const LevelMap& map);
    const std::vector<FieldEnemy>& Enemies() const;
    std::vector<WorldPosition> StrikeProjectile(WorldPosition from,
                                                WorldPosition to,
                                                float radiusX,
                                                float radiusY,
                                                int damage,
                                                std::unordered_set<std::uint64_t>& hitTargets,
                                                const LevelMap& map);

  private:
    EnemyStateMachine m_StateMachine;
    std::size_t m_UpdateStart = 0;
    bool m_SupportEnabled = true;
    std::vector<FieldEnemy> m_Enemies;
    std::vector<WorldPosition> m_ScriptDefeats;
    std::uint64_t m_NextId = 1;
};
