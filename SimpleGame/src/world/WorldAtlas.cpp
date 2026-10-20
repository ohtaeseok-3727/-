#include "stdafx.h"
#include "WorldAtlas.h"
#include <algorithm>
#include <cmath>
#include <limits>
#include <utility>

void WorldAtlas::Clear()
{
    m_Regions.clear();
}

void WorldAtlas::Add(std::string chapter,
                     MapDefinition definition,
                     WorldPosition origin,
                     WorldPosition departure)
{
    m_Regions.push_back({std::move(chapter), std::move(definition), origin, departure, false});
}

MapDefinition WorldAtlas::Build() const
{
    MapDefinition world;
    world.title = "긴 전쟁 · 변경과 북부 전선";
    world.openWorld = true;
    for (std::size_t i = 0; i < m_Regions.size(); ++i)
    {
        const auto& region = m_Regions[i];
        for (auto area : region.local.areas)
        {
            area.center = ToWorld(i, area.center);
            world.areas.push_back(std::move(area));
        }
        for (auto road : region.local.roads)
        {
            road.from = ToWorld(i, road.from);
            road.to = ToWorld(i, road.to);
            world.roads.push_back(road);
        }
        for (auto site : region.local.landmarks)
        {
            site.position = ToWorld(i, site.position);
            site.worldRegion = static_cast<int>(i);
            world.landmarks.push_back(std::move(site));
        }
        for (auto water : region.local.water)
        {
            water.center = ToWorld(i, water.center);
            world.water.push_back(water);
        }
        if (region.local.interior)
        {
            auto interior = *region.local.interior;
            interior.center = ToWorld(i, interior.center);
            world.interiors.push_back(interior);
        }
        for (auto encounter : region.local.encounters)
        {
            world.encounters.push_back(ToWorld(i, encounter));
        }
        // Rest sites are reachable entry anchors, never boss-arena checkpoints.
        world.landmarks.push_back({LandmarkKind::Inn,
                                   ToWorld(i, region.local.entry),
                                   "여행자 야영지",
                                   "Space: 휴식 · 체력과 마나 회복 · 복귀 지점 등록",
                                   0.f,
                                   -1,
                                   static_cast<int>(i)});
        if (i > 0)
        {
            const auto exit = m_Regions[i - 1].departure;
            world.roads.push_back({ToWorld(i - 1, exit), ToWorld(i, region.local.entry), 100.f});
        }
    }
    if (!m_Regions.empty())
    {
        world.entry = ToWorld(0, m_Regions.front().local.entry);
    }
    return world;
}

std::size_t WorldAtlas::RegionAt(WorldPosition world) const
{
    float best = (std::numeric_limits<float>::max)();
    std::size_t result = 0;
    for (std::size_t i = 0; i < m_Regions.size(); ++i)
    {
        const auto local = ToLocal(i, world);
        for (const auto& area : m_Regions[i].local.areas)
        {
            const float dx = (std::max)(0.f, std::abs(local.x - area.center.x) - area.halfWidth);
            const float dy = (std::max)(0.f, std::abs(local.y - area.center.y) - area.halfHeight);
            const float distance = dx * dx + dy * dy;
            if (distance < best)
            {
                best = distance;
                result = i;
            }
        }
    }
    return result;
}

std::optional<std::size_t> WorldAtlas::Find(const std::string& chapter, MapId map) const
{
    for (std::size_t i = 0; i < m_Regions.size(); ++i)
    {
        if (m_Regions[i].chapter == chapter && m_Regions[i].local.id == map)
        {
            return i;
        }
    }
    return std::nullopt;
}

WorldPosition WorldAtlas::ToWorld(std::size_t region, WorldPosition local) const
{
    const auto origin = m_Regions.at(region).origin;
    return {local.x + origin.x, local.y + origin.y};
}

WorldPosition WorldAtlas::ToLocal(std::size_t region, WorldPosition world) const
{
    const auto origin = m_Regions.at(region).origin;
    return {world.x - origin.x, world.y - origin.y};
}

const std::vector<WorldRegion>& WorldAtlas::Regions() const
{
    return m_Regions;
}

bool WorldAtlas::Discover(std::size_t region)
{
    auto& discovered = m_Regions.at(region).discovered;
    const bool first = !discovered;
    discovered = true;
    return first;
}
