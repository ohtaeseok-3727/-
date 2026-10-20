#pragma once

#include "LevelMap.h"
#include <optional>

struct WorldRegion
{
    std::string chapter;
    MapDefinition local;
    WorldPosition origin;
    WorldPosition departure;
    bool discovered = false;
};

struct WorldMarker
{
    WorldPosition position;
    std::string label;
    std::string hint;
};

// Authored regions share one world. Only nearby scenery is streamed by SceneGraph.
class WorldAtlas
{
  public:
    void Clear();
    void Add(std::string chapter,
             MapDefinition definition,
             WorldPosition origin,
             WorldPosition departure);
    MapDefinition Build() const;
    std::size_t RegionAt(WorldPosition world) const;
    std::optional<std::size_t> Find(const std::string& chapter, MapId map) const;
    WorldPosition ToWorld(std::size_t region, WorldPosition local) const;
    WorldPosition ToLocal(std::size_t region, WorldPosition world) const;
    const std::vector<WorldRegion>& Regions() const;
    bool Discover(std::size_t region);

  private:
    std::vector<WorldRegion> m_Regions;
};
