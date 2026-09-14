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
    return std::abs(x) < 230.f && std::abs(y) < 230.f;
}

bool LevelMap::IsRoad(float x, float y) const
{
    // Shared roads cross every chunk boundary, including negative coordinates.
    return IsTown(x, y) || std::abs(std::remainder(x, 512.f)) < 64.f ||
           std::abs(std::remainder(y, 512.f)) < 64.f;
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

    // Reserve the full collision footprint around the town and connected roads.
    if (std::abs(prop.x) < 280.f && std::abs(prop.y) < 280.f)
    {
        return false;
    }

    return std::abs(std::remainder(prop.x, 512.f)) >= 108.f &&
           std::abs(std::remainder(prop.y, 512.f)) >= 108.f;
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

    if (x == 0 && y == 0)
    {
        // Nonblocking landmarks mark the village, keeping the origin clear.
        props.push_back({130.f, 120.f, 3, 1.f});
        props.push_back({170.f, 120.f, 4, 1.f});
    }
    if ((x == -1 || x == 0) && y == -1)
    {
        props.push_back({x == -1 ? -128.f : 128.f, -128.f, 8, 1.f});
    }
    if (x == 0 && y == 1)
    {
        props.push_back({70.f, 980.f, 3, 1.f});
        props.push_back({110.f, 980.f, 4, 1.f});
    }

    return props;
}

bool LevelMap::Blocked(float x, float y) const
{
    for (const float houseX : {-128.f, 128.f})
    {
        const float dx = x - houseX;
        const float dy = y + 128.f;
        if (dx * dx + dy * dy < 44.f * 44.f)
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
    if (IsTown(x, y))
    {
        return "MERCENARY VILLAGE";
    }
    if (y > 600.f)
    {
        return "RIVERSIDE CAMP";
    }
    if (x < -250.f || y < -600.f)
    {
        return "WAGON ROAD WOODS";
    }
    if (x > 1500.f)
    {
        return "OLD BATTLEFIELD";
    }
    return "OUTER FARMLANDS";
}

std::uint32_t LevelMap::Seed() const
{
    return m_Seed;
}
