#include "stdafx.h"
#include "TutorialLayout.h"
#include <algorithm>
#include <cmath>

TutorialLayout::TutorialLayout()
{
    m_Areas = {{TutorialRegion::Village, "MERCENARY VILLAGE", {0, 0}, 420, 340},
               {TutorialRegion::Farmland, "NORTHERN FARMLANDS", {0, -960}, 520, 430},
               {TutorialRegion::Riverside, "RIVERSIDE CAMP", {-1360, -1040}, 460, 380},
               {TutorialRegion::Woods, "WAGON ROAD WOODS", {960, -1560}, 540, 460},
               {TutorialRegion::Battlefield, "OLD BATTLEFIELD", {-420, -2200}, 560, 400},
               {TutorialRegion::Outpost, "LEGION FOREST OUTPOST", {1960, -2240}, 360, 300},
               {TutorialRegion::MineEntrance, "ABANDONED MINE APPROACH", {2860, -2640}, 340, 300},
               {TutorialRegion::GuardTunnel, "GUARD TUNNEL", {3680, -3000}, 130, 240},
               {TutorialRegion::CentralMine, "CENTRAL MINE", {3680, -3640}, 260, 220},
               {TutorialRegion::SupplyStore, "SUPPLY STORE", {3160, -4240}, 200, 210},
               {TutorialRegion::WorkTunnel, "WORKING TUNNEL", {4200, -4240}, 190, 210},
               {TutorialRegion::CommandHall, "COMMAND QUARTERS", {3680, -4860}, 230, 200},
               {TutorialRegion::Arena, "COMMANDER ARENA", {3680, -5440}, 280, 240}};

    // Village -> northern first contract. Three investigations can be reached in any order.
    m_Roads = {{{0, 0}, {0, -360}, 72},
               {{0, -360}, {-80, -660}, 72},
               {{-80, -660}, {0, -960}, 80},
               {{0, -960}, {180, -1160}, 64},
               {{0, -920}, {-640, -920}, 64},
               {{-640, -920}, {-1360, -1040}, 64},
               {{0, -1040}, {580, -1240}, 64},
               {{580, -1240}, {960, -1560}, 64},
               {{0, -1180}, {-160, -1660}, 64},
               {{-160, -1660}, {-420, -2200}, 64},
               {{-1360, -1040}, {-1480, -1740}, 52},
               {{-1480, -1740}, {-420, -2200}, 52},
               {{-420, -2200}, {400, -2100}, 56},
               {{400, -2100}, {960, -1560}, 56},
               // Short main gate approach and longer, less populated east flank.
               {{960, -1560}, {1420, -1780}, 64},
               {{1420, -1780}, {1720, -2020}, 64},
               {{1720, -2020}, {1960, -2240}, 72},
               {{960, -1560}, {1600, -1340}, 48},
               {{1600, -1340}, {2360, -1620}, 48},
               {{2360, -1620}, {2390, -2300}, 48},
               {{2390, -2300}, {1960, -2240}, 48},
               {{1960, -2240}, {2480, -2460}, 60},
               {{2480, -2460}, {2860, -2640}, 60},
               // A return road reaches the village without crossing the outpost again.
               {{2480, -2460}, {2280, -900}, 52},
               {{2280, -900}, {960, -240}, 52},
               {{960, -240}, {320, 0}, 52},
               // Mine rooms: central hub -> either branch -> command -> arena.
               {{2860, -2640}, {3260, -2800}, 60},
               {{3260, -2800}, {3680, -2800}, 56},
               {{3680, -2800}, {3680, -3000}, 56},
               {{3680, -3000}, {3680, -3640}, 48},
               {{3680, -3640}, {3160, -3920}, 48},
               {{3160, -3920}, {3160, -4240}, 48},
               {{3680, -3640}, {4200, -3920}, 48},
               {{4200, -3920}, {4200, -4240}, 48},
               {{3160, -4240}, {3200, -4640}, 48},
               {{3200, -4640}, {3680, -4860}, 48},
               {{4200, -4240}, {4160, -4640}, 48},
               {{4160, -4640}, {3680, -4860}, 48},
               {{3680, -4860}, {3680, -5440}, 60}};

    m_Landmarks = {
        {LandmarkKind::Board,
         {0, -100},
         "CONTRACT BOARD",
         "First contract: clear the NORTHERN fields. The abandoned farm is beyond them.",
         0},
        {LandmarkKind::Shop,
         {240, -150},
         "POTION & GEAR SHOP",
         "The village's preparation stop: potions and basic equipment.",
         38},
        {LandmarkKind::Trader,
         {240, 150},
         "LOOT MERCHANT",
         "A place to bring recovered monster loot after each expedition.",
         34},
        {LandmarkKind::Inn,
         {-240, 120},
         "INN / REST STOP",
         "Rest and regroup in the village before following the mine trail.",
         38},
        {LandmarkKind::Veteran,
         {-160, -100},
         "VETERAN MERCENARY",
         "Bring the farm, river, woods and battlefield clues back here.",
         0},
        {LandmarkKind::Farmhouse,
         {260, -1260},
         "ABANDONED FARM",
         "Some valuables remain. Food and tools are gone. Wagon tracks lead into the woods.",
         38},
        {LandmarkKind::Tracks,
         {510, -1190},
         "WAGON TRACKS",
         "Heavy loads were carried east, toward the wagon-road woods.",
         0},
        {LandmarkKind::Wagon,
         {-1410, -1080},
         "BROKEN MERCHANT WAGON",
         "Coins were left behind. Food and metal tools were taken.",
         24},
        {LandmarkKind::Supplies,
         {1020, -1640},
         "SORTED STOLEN SUPPLIES",
         "The stolen goods were sorted and moved onward, not simply abandoned.",
         22},
        {LandmarkKind::Banner,
         {-370, -2290},
         "LEGION MARK",
         "The same mark appears on the supplies, wagon and battlefield remains.",
         0},
        {LandmarkKind::Palisade,
         {1820, -2160},
         "OUTPOST MAIN GATE",
         "The short route meets the main guards. The eastern trail goes around them.",
         0},
        {LandmarkKind::Records,
         {1960, -2320},
         "TRANSPORT RECORDS",
         "Supply routes converge on the abandoned mine. Return to the veteran before setting out.",
         0},
        {LandmarkKind::MineGate,
         {2860, -2700},
         "ABANDONED MINE",
         "Entrance -> guard tunnel -> central mine -> either branch -> command quarters.",
         0},
        {LandmarkKind::Supplies,
         {3060, -4300},
         "STOLEN FOOD & EQUIPMENT",
         "Farm food, merchant tools and human equipment are stored together here.",
         22},
        {LandmarkKind::Workbench,
         {4300, -4300},
         "WAR SUPPLY WORKBENCH",
         "The mine is being used to prepare war supplies.",
         22},
        {LandmarkKind::CommandGate,
         {3680, -5110},
         "COMMAND GATE",
         "Both mine branches rejoin here. The commander's chamber lies beyond.",
         0},
        {LandmarkKind::Arena,
         {3680, -5440},
         "COMMANDER CHAMBER",
         "The first named encounter belongs here, before the hero's arrival in the village.",
         0},
        {LandmarkKind::Sign,
         {80, -370},
         "NORTH: FIRST CONTRACT",
         "Northern fields ahead. Return to the village after investigating the abandoned farm.",
         0},
        {LandmarkKind::Sign,
         {140, -1000},
         "THREE INVESTIGATIONS",
         "West: riverside wagon. Northeast: supply woods. Northwest: old battlefield.",
         0},
        {LandmarkKind::Sign,
         {1570, -1430},
         "OUTPOST FLANK TRAIL",
         "This longer eastern trail has fewer guards than the main gate road.",
         0},
        {LandmarkKind::Sign,
         {2390, -2380},
         "VILLAGE RETURN ROAD",
         "Follow the southern road back to the village to prepare for the mine.",
         0}};

    // First five groups belong to the farming contract; none occupy the flank trail.
    m_Encounters = {{90, -630},
                    {-170, -790},
                    {160, -940},
                    {-190, -1100},
                    {70, -1220},
                    {-1280, -1000},
                    {900, -1480},
                    {-480, -2180},
                    {1700, -1980},
                    {1900, -2200},
                    {2780, -2560},
                    {3680, -3010},
                    {3660, -3620},
                    {3160, -4170},
                    {4200, -4170},
                    {3690, -4790}};
}

float TutorialLayout::RoadDistanceSquared(WorldPosition p, const TutorialRoad& road)
{
    const float dx = road.to.x - road.from.x;
    const float dy = road.to.y - road.from.y;
    const float lengthSquared = dx * dx + dy * dy;
    const float t =
        lengthSquared > 0.f
            ? (std::max)(
                  0.f,
                  (std::min)(1.f,
                             ((p.x - road.from.x) * dx + (p.y - road.from.y) * dy) / lengthSquared))
            : 0.f;
    const float px = p.x - road.from.x - dx * t;
    const float py = p.y - road.from.y - dy * t;
    return px * px + py * py;
}

bool TutorialLayout::OnRoad(WorldPosition p, float margin) const
{
    for (const auto& road : m_Roads)
    {
        const float width = road.halfWidth + margin;
        if (width >= 0.f && RoadDistanceSquared(p, road) <= width * width)
        {
            return true;
        }
    }
    return false;
}

bool TutorialLayout::Reserved(WorldPosition p, float margin) const
{
    if (OnRoad(p, margin))
    {
        return true;
    }
    for (const auto& landmark : m_Landmarks)
    {
        const float dx = p.x - landmark.position.x;
        const float dy = p.y - landmark.position.y;
        if (dx * dx + dy * dy < (110.f + margin) * (110.f + margin))
        {
            return true;
        }
    }
    for (const auto& center : m_Encounters)
    {
        const float dx = p.x - center.x;
        const float dy = p.y - center.y;
        if (dx * dx + dy * dy < (100.f + margin) * (100.f + margin))
        {
            return true;
        }
    }
    return false;
}

bool TutorialLayout::River(WorldPosition p, float margin) const
{
    if (p.y < -2400.f - margin || p.y > -400.f + margin)
    {
        return false;
    }
    const float center = -1100.f + (p.y + 1000.f) * .12f;
    return std::abs(p.x - center) < 64.f + margin;
}

bool TutorialLayout::InMineBounds(WorldPosition p) const
{
    return p.x >= 2900.f && p.x <= 4620.f && p.y <= -2800.f && p.y >= -5900.f;
}

bool TutorialLayout::MineFloor(WorldPosition p, float inset) const
{
    for (const auto& area : m_Areas)
    {
        if (area.id >= TutorialRegion::GuardTunnel &&
            std::abs(p.x - area.center.x) <= area.halfWidth - inset &&
            std::abs(p.y - area.center.y) <= area.halfHeight - inset)
        {
            return true;
        }
    }
    return OnRoad(p, -inset);
}

TutorialRegion TutorialLayout::RegionAt(WorldPosition p) const
{
    for (const auto& area : m_Areas)
    {
        if (std::abs(p.x - area.center.x) <= area.halfWidth &&
            std::abs(p.y - area.center.y) <= area.halfHeight)
        {
            return area.id;
        }
    }
    return TutorialRegion::Wilds;
}

const char* TutorialLayout::RegionName(WorldPosition p) const
{
    const auto id = RegionAt(p);
    for (const auto& area : m_Areas)
    {
        if (area.id == id)
        {
            return area.name;
        }
    }
    return InMineBounds(p) ? "MINE PASSAGE" : "CONNECTING TRAIL";
}

const TutorialLandmark* TutorialLayout::NearbyLandmark(WorldPosition p) const
{
    const TutorialLandmark* closest = nullptr;
    float best = 110.f * 110.f;
    for (const auto& landmark : m_Landmarks)
    {
        const float dx = p.x - landmark.position.x;
        const float dy = p.y - landmark.position.y;
        const float distance = dx * dx + dy * dy;
        if (distance < best)
        {
            closest = &landmark;
            best = distance;
        }
    }
    return closest;
}

const std::vector<TutorialArea>& TutorialLayout::Areas() const
{
    return m_Areas;
}

const std::vector<TutorialRoad>& TutorialLayout::Roads() const
{
    return m_Roads;
}

const std::vector<TutorialLandmark>& TutorialLayout::Landmarks() const
{
    return m_Landmarks;
}

const std::vector<WorldPosition>& TutorialLayout::EncounterCenters() const
{
    return m_Encounters;
}
