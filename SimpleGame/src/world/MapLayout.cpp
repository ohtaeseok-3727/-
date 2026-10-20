#include "stdafx.h"
#include "MapLayout.h"

#include <algorithm>
#include <cmath>
#include <utility>

void MapLayout::Load(MapDefinition definition)
{
    m_Definition = std::move(definition);
}

MapId MapLayout::Map() const
{
    return m_Definition.id;
}

const MapDefinition& MapLayout::Definition() const
{
    return m_Definition;
}

bool MapRectangle::Contains(WorldPosition p, float margin) const
{
    return std::abs(p.x - center.x) <= halfWidth + margin &&
           std::abs(p.y - center.y) <= halfHeight + margin;
}

float MapLayout::RoadDistanceSquared(WorldPosition p, const MapRoad& road)
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

bool MapLayout::OnRoad(WorldPosition p, float margin) const
{
    for (const auto& road : m_Definition.roads)
    {
        const float width = road.halfWidth + margin;
        if (width >= 0.f && RoadDistanceSquared(p, road) <= width * width)
        {
            return true;
        }
    }
    return false;
}

bool MapLayout::Reserved(WorldPosition p, float margin) const
{
    if (OnRoad(p, margin))
    {
        return true;
    }
    for (const auto& landmark : m_Definition.landmarks)
    {
        const float dx = p.x - landmark.position.x;
        const float dy = p.y - landmark.position.y;
        if (dx * dx + dy * dy < (110.f + margin) * (110.f + margin))
        {
            return true;
        }
    }
    for (const auto& center : m_Definition.encounters)
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

bool MapLayout::Water(WorldPosition p, float margin) const
{
    return std::any_of(m_Definition.water.begin(),
                       m_Definition.water.end(),
                       [p, margin](const MapRectangle& area)
                       {
                           return area.Contains(p, margin);
                       });
}

bool MapLayout::Interior(WorldPosition p) const
{
    return (m_Definition.interior && m_Definition.interior->Contains(p)) ||
           std::any_of(m_Definition.interiors.begin(),
                       m_Definition.interiors.end(),
                       [p](const MapRectangle& area)
                       {
                           return area.Contains(p);
                       });
}

bool MapLayout::InteriorFloor(WorldPosition p, float inset) const
{
    return Interior(p) && Walkable(p, inset);
}

bool MapLayout::Walkable(WorldPosition p, float inset) const
{
    if (m_Definition.openWorld && !Interior(p))
    {
        return true;
    }
    for (const auto& area : m_Definition.areas)
    {
        if (std::abs(p.x - area.center.x) <= area.halfWidth - inset &&
            std::abs(p.y - area.center.y) <= area.halfHeight - inset)
        {
            return true;
        }
    }
    return OnRoad(p, -inset);
}

MapRegion MapLayout::RegionAt(WorldPosition p) const
{
    for (const auto& area : m_Definition.areas)
    {
        if (std::abs(p.x - area.center.x) <= area.halfWidth &&
            std::abs(p.y - area.center.y) <= area.halfHeight)
        {
            return area.id;
        }
    }
    return MapRegion::Wilds;
}

const char* MapLayout::RegionName(WorldPosition p) const
{
    for (const auto& area : m_Definition.areas)
    {
        if (std::abs(p.x - area.center.x) <= area.halfWidth &&
            std::abs(p.y - area.center.y) <= area.halfHeight)
        {
            return area.name.c_str();
        }
    }
    return Interior(p) ? m_Definition.wallName.c_str() : "변경의 야생지";
}

const MapLandmark* MapLayout::NearbyLandmark(WorldPosition p) const
{
    const MapLandmark* closest = nullptr;
    float best = 110.f * 110.f;
    for (const auto& landmark : m_Definition.landmarks)
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

const std::vector<MapArea>& MapLayout::Areas() const
{
    return m_Definition.areas;
}

const std::vector<MapRoad>& MapLayout::Roads() const
{
    return m_Definition.roads;
}

const std::vector<MapLandmark>& MapLayout::Landmarks() const
{
    return m_Definition.landmarks;
}

const std::vector<WorldPosition>& MapLayout::EncounterCenters() const
{
    return m_Definition.encounters;
}
