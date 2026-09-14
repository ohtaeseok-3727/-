#include "stdafx.h"
#include "LevelMap.h"
#include <cmath>

void LevelMap::Generate(std::uint32_t seed)
{
    m_Seed = seed;
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
    return m_Layout.RegionAt({x, y}) == TutorialRegion::Village;
}

bool LevelMap::IsRoad(float x, float y) const
{
    return IsTown(x, y) || m_Layout.OnRoad({x, y});
}

bool LevelMap::CellProp(int x, int y, WorldProp& prop) const
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
    // Keep roads, clues, encounters, river crossings and room entrances clear.
    if ((std::abs(prop.x) < 470.f && std::abs(prop.y) < 390.f) || m_Layout.Reserved(p, 44.f) ||
        m_Layout.River(p, 100.f) || m_Layout.InMineBounds({p.x - 44.f, p.y + 44.f}) ||
        m_Layout.InMineBounds({p.x + 44.f, p.y - 44.f}))
    {
        return false;
    }
    if (m_Layout.RegionAt(p) == TutorialRegion::Woods)
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

    return props;
}

bool LevelMap::Blocked(float x, float y) const
{
    const WorldPosition p{x, y};
    // Bridges have a player-radius inset; river water and mine walls cannot be crossed.
    if ((m_Layout.River(p, 11.f) && !m_Layout.OnRoad(p, -11.f)) ||
        (m_Layout.InMineBounds(p) && !m_Layout.MineFloor(p, 11.f)))
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

const TutorialLayout& LevelMap::Layout() const
{
    return m_Layout;
}

std::uint32_t LevelMap::Seed() const
{
    return m_Seed;
}
