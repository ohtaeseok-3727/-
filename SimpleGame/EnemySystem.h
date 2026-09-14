#pragma once

#include "LevelMap.h"
#include <random>
#include <vector>

struct FieldEnemy
{
    WorldPosition position;
    int health = LevelTuning::EnemyHealth;
    float cooldown = 1.f;
    float windup = 0.f;
    float respawn = 0.f;
    float hitFlash = 0.f;
};

class EnemySystem
{
  public:
    void Reset(unsigned seed, const LevelMap& map);
    int Update(float dt, WorldPosition player, const LevelMap& map);
    std::vector<WorldPosition> Strike(WorldPosition player,
                                      bool faceLeft,
                                      int damage,
                                      const LevelMap& map);
    const std::vector<FieldEnemy>& Enemies() const;

  private:
    void Spawn(FieldEnemy& enemy, WorldPosition player, const LevelMap& map);
    std::mt19937 m_Random{1};
    std::vector<FieldEnemy> m_Enemies;
};
