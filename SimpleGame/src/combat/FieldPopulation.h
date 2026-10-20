#pragma once
#include "EnemySystem.h"
#include <array>
#include <vector>

// Optional outdoor packs have IDs outside the authored chapter encounter range.
class FieldPopulation
{
  public:
    void Populate(const LevelMap& map, EnemySystem& enemies);
    void Update(float dt, WorldPosition player, EnemySystem& enemies);

  private:
    struct Pack
    {
        std::array<WorldPosition, 3> positions;
        std::size_t encounter;
        float cooldown = 45.f;
        int health = LevelTuning::EnemyHealth;
    };

    void Spawn(const Pack& pack, EnemySystem& enemies) const;
    std::vector<Pack> m_Packs;
    float m_Elapsed = 0.f;
};
