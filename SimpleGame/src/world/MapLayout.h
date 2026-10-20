#pragma once

#include "LevelTypes.h"
#include <vector>
#include <string>
#include <optional>

using MapId = int;
using ObjectId = int;

enum class MapRegion
{
    Wilds,
    Village,
    Farmland,
    Riverside,
    Woods,
    Battlefield,
    Outpost,
    MineEntrance,
    GuardTunnel,
    CentralMine,
    SupplyStore,
    WorkTunnel,
    CommandHall,
    Arena
};

enum class LandmarkKind
{
    Board,
    Shop,
    Trader,
    Inn,
    Veteran,
    Farmhouse,
    Tracks,
    Wagon,
    Supplies,
    Banner,
    Palisade,
    Records,
    MineGate,
    Workbench,
    CommandGate,
    Arena,
    Sign
};

struct MapArea
{
    MapRegion id;
    std::string name;
    WorldPosition center;
    float halfWidth;
    float halfHeight;
};

struct MapRoad
{
    WorldPosition from;
    WorldPosition to;
    float halfWidth;
};

struct MapLandmark
{
    LandmarkKind kind;
    WorldPosition position;
    std::string name;
    std::string description;
    float radius;
    ObjectId object = 0;
    int worldRegion = -1;
};

// Geometry and presentation metadata travel together; no chapter-specific switches here.
struct MapRectangle
{
    WorldPosition center;
    float halfWidth = 0.f;
    float halfHeight = 0.f;
    bool Contains(WorldPosition p, float margin = 0.f) const;
};

enum class BuildingStyle
{
    House,
    Workshop,
    Archive,
    Monastery,
    Wall,
    Bookshelf
};

// A separately placed object: footprint is shared by collision and the building view.
struct MapBuilding
{
    int id;
    std::string name;
    MapRectangle footprint;
    float height;
    BuildingStyle style;
};

struct MapDistrict
{
    std::string name;
    MapRectangle bounds;
    int paving = 0;
};

struct MapPortal
{
    ObjectId object;
    MapId destination;
    WorldPosition arrival;
};

struct MapDefinition
{
    MapId id = 0;
    std::string title;
    WorldPosition entry;
    std::vector<MapArea> areas;
    std::vector<MapRoad> roads;
    std::vector<MapLandmark> landmarks;
    std::vector<WorldPosition> encounters;
    std::vector<MapRectangle> water;
    std::optional<MapRectangle> interior;
    std::string wallName = "벽면";
    bool openWorld = false;
    std::vector<MapRectangle> interiors;
    int fieldEnemyHealth = LevelTuning::EnemyHealth;
    std::vector<MapBuilding> buildings;
    std::vector<MapDistrict> districts;
    std::vector<MapPortal> portals;
    bool portalInterior = false;
};

class MapLayout
{
  public:
    void Load(MapDefinition definition);
    MapId Map() const;
    const MapDefinition& Definition() const;
    const std::vector<MapArea>& Areas() const;
    const std::vector<MapRoad>& Roads() const;
    const std::vector<MapLandmark>& Landmarks() const;
    const std::vector<WorldPosition>& EncounterCenters() const;
    MapRegion RegionAt(WorldPosition p) const;
    const char* RegionName(WorldPosition p) const;
    bool OnRoad(WorldPosition p, float margin = 0.f) const;
    bool Reserved(WorldPosition p, float margin = 0.f) const;
    bool Water(WorldPosition p, float margin = 0.f) const;
    bool Interior(WorldPosition p) const;
    bool InteriorFloor(WorldPosition p, float inset = 0.f) const;
    bool Walkable(WorldPosition p, float inset = 0.f) const;
    bool BuildingBlocked(WorldPosition p, float margin = 0.f) const;
    const MapDistrict* DistrictAt(WorldPosition p) const;
    const MapLandmark* NearbyLandmark(WorldPosition p) const;

  private:
    static float RoadDistanceSquared(WorldPosition p, const MapRoad& road);
    MapDefinition m_Definition;
};
