#pragma once

#include "LevelTypes.h"
#include <vector>
#include <cstdint>

class LevelMap
{
  public:
    void Generate(std::uint32_t seed);
    std::vector<WorldProp> ChunkObjects(int x, int y) const;
    bool Blocked(float x, float y) const;
    bool ClearPath(WorldPosition from, WorldPosition to) const;
    bool IsRoad(float x, float y) const;
    bool IsTown(float x, float y) const;
    const char* RegionName(float x, float y) const;
    std::uint32_t Seed() const;
    std::uint32_t Hash(int x, int y, unsigned salt = 0) const;

  private:
    bool CellProp(int x, int y, WorldProp& prop) const;
    std::uint32_t m_Seed = 1;
};
