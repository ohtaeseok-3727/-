#pragma once

#include "LevelTypes.h"
#include <random>
#include <vector>

struct LootDrop
{
    WorldPosition position;
    LootKind kind;
    int value;
    float age = 0.f;
    bool attracted = false;
};

class LootSystem
{
  public:
    void Reset(unsigned seed);
    void Drop(WorldPosition position, int killCount, int level);
    std::vector<LootDrop> Update(float dt, WorldPosition player);
    const std::vector<LootDrop>& Drops() const;

  private:
    std::mt19937 m_Random{1};
    std::vector<LootDrop> m_Drops;
};
