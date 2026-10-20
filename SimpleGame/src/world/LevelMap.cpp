#include "stdafx.h"
#include "LevelMap.h"
#include <algorithm>
#include <cmath>
#include <utility>

void LevelMap::Generate(std::uint32_t seed)
{
    m_PropCells.clear();
    m_Seed = seed;
    ++m_Revision;
}

void LevelMap::Load(std::string chapter, MapDefinition definition)
{
    m_PropCells.clear();
    m_WorldGeometry = nullptr;
    m_Layout.Load(std::move(definition));
    m_ChapterId = std::move(chapter);
    ++m_Revision; // Also invalidates paths when two chapters use the same local map ID.
}

const std::string& LevelMap::ChapterId() const
{
    return m_ChapterId;
}

MapId LevelMap::Map() const
{
    return m_Layout.Map();
}

std::uint64_t LevelMap::Revision() const
{
    return m_Revision;
}

std::uint32_t LevelMap::Hash(int x, int y, unsigned salt) const
{
    std::uint32_t h = static_cast<std::uint32_t>(x) * 374761393u ^
                      static_cast<std::uint32_t>(y) * 668265263u ^ m_Seed ^ salt;
    h = (h ^ (h >> 13)) * 1274126177u;
    return h ^ (h >> 16);
}

bool LevelMap::IsTown(float x, float y) const
{
    return m_Layout.RegionAt({x, y}) == MapRegion::Village;
}

bool LevelMap::IsRoad(float x, float y) const
{
    return IsTown(x, y) || m_Layout.OnRoad({x, y});
}

bool LevelMap::CellProp(int x, int y, WorldProp& prop) const
{
    if (m_PropCells.empty())
    {
        m_PropCells.resize(4096);
    }
    const auto slot = (static_cast<unsigned>(y) & 63u) * 64u + (static_cast<unsigned>(x) & 63u);
    auto& cell = m_PropCells[slot];
    if (!cell.valid || cell.x != x || cell.y != y)
    {
        cell.x = x;
        cell.y = y;
        cell.occupied = GenerateCellProp(x, y, cell.prop);
        cell.valid = true;
    }
    prop = cell.prop;
    return cell.occupied;
}

bool LevelMap::GenerateCellProp(int x, int y, WorldProp& prop) const
{
    const auto h = Hash(x, y);
    if (h % 100 < 30)
    {
        return false;
    }

    prop = {x * 128.f + 64.f + float((h >> 8) % 17) - 8.f,
            y * 128.f + 64.f + float((h >> 16) % 17) - 8.f,
            int((h >> 24) % 3),
            .65f + float((h >> 5) % 11) * .01f};

    const WorldPosition p{prop.x, prop.y};
    if (m_Layout.DistrictAt(p))
    {
        return false; // Authored city blocks and archive rooms own all their decoration.
    }
    // Keep roads, clues, encounters, river crossings and room entrances clear.
    if (!m_Layout.Walkable(p, 44.f) || m_Layout.Reserved(p, 44.f) || m_Layout.Water(p, 100.f))
    {
        return false;
    }
    if (m_Layout.RegionAt(p) == MapRegion::Woods)
    {
        prop.kind = 0;
    }
    return true;
}

std::vector<WorldProp> LevelMap::ChunkObjects(int x, int y) const
{
    std::vector<WorldProp> props;
    for (int cy = y * 4; cy < y * 4 + 4; ++cy)
    {
        for (int cx = x * 4; cx < x * 4 + 4; ++cx)
        {
            WorldProp prop{};
            if (CellProp(cx, cy, prop))
            {
                props.push_back(prop);
            }
        }
    }

    const auto& landmarks = m_Layout.Landmarks();
    for (std::size_t i = 0; i < landmarks.size(); ++i)
    {
        const auto p = landmarks[i].position;
        if (static_cast<int>(std::floor(p.x / 512.f)) == x &&
            static_cast<int>(std::floor(p.y / 512.f)) == y)
        {
            props.push_back({p.x, p.y, 9, 1.f, static_cast<int>(i)});
        }
    }

    const auto& buildings = m_Layout.Definition().buildings;
    for (std::size_t i = 0; i < buildings.size(); ++i)
    {
        const auto& bounds = buildings[i].footprint;
        const WorldPosition anchor{bounds.center.x, bounds.center.y + bounds.halfHeight};
        if (static_cast<int>(std::floor(anchor.x / 512.f)) == x &&
            static_cast<int>(std::floor(anchor.y / 512.f)) == y)
        {
            props.push_back({anchor.x,
                             anchor.y,
                             13,
                             (std::max)(bounds.halfWidth, bounds.halfHeight + buildings[i].height),
                             static_cast<int>(i)});
        }
    }
    return props;
}

bool LevelMap::Blocked(float x, float y) const
{
    if (m_WorldGeometry)
    {
        return m_WorldGeometry->Blocked(x + m_WorldOrigin.x, y + m_WorldOrigin.y);
    }
    const WorldPosition p{x, y};
    if (m_Layout.BuildingBlocked(p, 11.f))
    {
        return true;
    }
    // Every map is an authored, bounded walkable topology. Water and mine walls are extra terrain limits.
    if (!m_Layout.Walkable(p, 11.f) || (m_Layout.Water(p, 11.f) && !m_Layout.OnRoad(p, -11.f)) ||
        (m_Layout.Interior(p) && !m_Layout.InteriorFloor(p, 11.f)))
    {
        return true;
    }
    for (const auto& landmark : m_Layout.Landmarks())
    {
        if (landmark.radius <= 0.f)
        {
            continue;
        }
        const float dx = x - landmark.position.x;
        const float dy = y - landmark.position.y;
        const float radius = landmark.radius + 11.f;
        if (dx * dx + dy * dy < radius * radius)
        {
            return true;
        }
    }
    const int cx = static_cast<int>(std::floor(x / 128.f));
    const int cy = static_cast<int>(std::floor(y / 128.f));
    for (int row = cy - 1; row <= cy + 1; ++row)
    {
        for (int col = cx - 1; col <= cx + 1; ++col)
        {
            WorldProp prop{};
            if (!CellProp(col, row, prop))
            {
                continue;
            }

            const float dx = x - prop.x;
            const float dy = y - prop.y;
            // Includes the player's 11-unit radius. Centers are at least 112 apart:
            // disjoint 44-radius disks leave >=24 units of traversable space.
            if (dx * dx + dy * dy < 44.f * 44.f)
            {
                return true;
            }
        }
    }

    return false;
}

void LevelMap::UseWorldGeometry(const LevelMap& world, WorldPosition origin)
{
    m_WorldGeometry = &world;
    m_WorldOrigin = origin;
}

bool LevelMap::ClearPath(WorldPosition from, WorldPosition to) const
{
    const float dx = to.x - from.x;
    const float dy = to.y - from.y;
    const int steps = static_cast<int>(std::ceil(std::sqrt(dx * dx + dy * dy) / 4.f)) + 1;
    for (int i = 0; i <= steps; ++i)
    {
        const float t = float(i) / steps;
        if (Blocked(from.x + dx * t, from.y + dy * t))
        {
            return false;
        }
    }

    return true;
}

const char* LevelMap::RegionName(float x, float y) const
{
    return m_Layout.RegionName({x, y});
}

const MapLayout& LevelMap::Layout() const
{
    return m_Layout;
}

std::uint32_t LevelMap::Seed() const
{
    return m_Seed;
}
