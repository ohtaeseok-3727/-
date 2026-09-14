#pragma once

#include "LevelTypes.h"
#include <vector>

enum class TutorialRegion
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

struct TutorialArea
{
    TutorialRegion id;
    const char* name;
    WorldPosition center;
    float halfWidth;
    float halfHeight;
};

struct TutorialRoad
{
    WorldPosition from;
    WorldPosition to;
    float halfWidth;
};

struct TutorialLandmark
{
    LandmarkKind kind;
    WorldPosition position;
    const char* name;
    const char* description;
    float radius;
};

// Authored scenario topology; coordinates are implementation choices, not story canon.
class TutorialLayout
{
  public:
    TutorialLayout();
    const std::vector<TutorialArea>& Areas() const;
    const std::vector<TutorialRoad>& Roads() const;
    const std::vector<TutorialLandmark>& Landmarks() const;
    const std::vector<WorldPosition>& EncounterCenters() const;
    TutorialRegion RegionAt(WorldPosition p) const;
    const char* RegionName(WorldPosition p) const;
    bool OnRoad(WorldPosition p, float margin = 0.f) const;
    bool Reserved(WorldPosition p, float margin = 0.f) const;
    bool River(WorldPosition p, float margin = 0.f) const;
    bool InMineBounds(WorldPosition p) const;
    bool MineFloor(WorldPosition p, float inset = 0.f) const;
    const TutorialLandmark* NearbyLandmark(WorldPosition p) const;

  private:
    static float RoadDistanceSquared(WorldPosition p, const TutorialRoad& road);
    std::vector<TutorialArea> m_Areas;
    std::vector<TutorialRoad> m_Roads;
    std::vector<TutorialLandmark> m_Landmarks;
    std::vector<WorldPosition> m_Encounters;
};
