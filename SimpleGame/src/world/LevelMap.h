#pragma once

#include "LevelTypes.h"
#include "MapLayout.h"
#include <vector>
#include <cstdint>

class LevelMap
{
  public:
    void Generate(std::uint32_t seed);
    void Load(std::string chapter, MapDefinition definition);
    const std::string& ChapterId() const;
    MapId Map() const;
    std::uint64_t Revision() const;
    std::vector<WorldProp> ChunkObjects(int x, int y) const;
    bool Blocked(float x, float y) const;
    bool ClearPath(WorldPosition from, WorldPosition to) const;
    bool IsRoad(float x, float y) const;
    bool IsTown(float x, float y) const;
    const char* RegionName(float x, float y) const;
    std::uint32_t Seed() const;
    std::uint32_t Hash(int x, int y, unsigned salt = 0) const;
    const MapLayout& Layout() const;

  private:
    bool CellProp(int x, int y, WorldProp& prop) const;
    std::uint32_t m_Seed = 1;
    std::uint64_t m_Revision = 0;
    MapLayout m_Layout;
    std::string m_ChapterId;
};
