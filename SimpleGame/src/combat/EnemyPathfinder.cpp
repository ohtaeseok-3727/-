#include "stdafx.h"
#include "EnemyPathfinder.h"
#include <algorithm>
#include <cmath>
#include <limits>
#include <queue>

namespace
{
    constexpr float CellSize = 24.f;
    constexpr int Margin = 16;
    constexpr int MaxExpanded = 4096;

    struct OpenNode
    {
        int index;
        float cost;
        float estimate;

        bool operator<(const OpenNode& other) const
        {
            return estimate > other.estimate;
        }
    };
}

bool EnemyPathfinder::FindPath(WorldPosition start,
                               WorldPosition goal,
                               const LevelMap& map,
                               std::vector<WorldPosition>& path) const
{
    path.clear();
    if (map.Blocked(start.x, start.y) || map.Blocked(goal.x, goal.y))
    {
        return false;
    }

    // Open ground needs no grid allocation or A* expansion.
    if (map.ClearPath(start, goal))
    {
        path.push_back(goal);
        return true;
    }

    // The grid is anchored at the actual start, avoiding a snap through nearby walls.
    const int gx = static_cast<int>(std::round((goal.x - start.x) / CellSize));
    const int gy = static_cast<int>(std::round((goal.y - start.y) / CellSize));
    const int left = (std::min)(0, gx) - Margin;
    const int top = (std::min)(0, gy) - Margin;
    const int width = std::abs(gx) + Margin * 2 + 1;
    const int height = std::abs(gy) + Margin * 2 + 1;
    // Keep memory bounded even if a future caller supplies a distant target.
    if (width > 128 || height > 128)
    {
        return false;
    }
    const auto position = [&](int index)
    {
        return WorldPosition{start.x + (index % width + left) * CellSize,
                             start.y + (index / width + top) * CellSize};
    };
    const auto heuristic = [&](WorldPosition p)
    {
        const float dx = goal.x - p.x;
        const float dy = goal.y - p.y;
        return (std::max)(0.f, std::sqrt(dx * dx + dy * dy) - CellSize);
    };
    std::vector<float> costs(width * height, (std::numeric_limits<float>::max)());
    std::vector<int> parents(width * height, -1);
    std::priority_queue<OpenNode> open;
    const int origin = -top * width - left;
    costs[origin] = 0.f;
    open.push({origin, 0.f, heuristic(start)});
    int expanded = 0;
    while (!open.empty() && expanded < MaxExpanded)
    {
        const auto current = open.top();
        open.pop();
        if (current.cost > costs[current.index])
        {
            continue;
        }
        ++expanded;
        const auto from = position(current.index);
        if (heuristic(from) <= .001f && map.ClearPath(from, goal))
        {
            path.push_back(goal);
            for (int index = current.index; index != origin; index = parents[index])
            {
                path.push_back(position(index));
            }
            std::reverse(path.begin(), path.end());
            return true;
        }
        for (int dy = -1; dy <= 1; ++dy)
        {
            for (int dx = -1; dx <= 1; ++dx)
            {
                const int x = current.index % width + dx;
                const int y = current.index / width + dy;
                if ((dx == 0 && dy == 0) || x < 0 || y < 0 || x >= width || y >= height)
                {
                    continue;
                }
                const int next = y * width + x;
                const float cost =
                    current.cost + CellSize * (dx != 0 && dy != 0 ? 1.41421356f : 1.f);
                if (cost >= costs[next])
                {
                    continue;
                }
                const auto to = position(next);
                // Validate the whole edge and both side cells to prevent diagonal corner cutting.
                if (!map.ClearPath(from, to) ||
                    (dx != 0 && dy != 0 &&
                     (map.Blocked(to.x, from.y) || map.Blocked(from.x, to.y))))
                {
                    continue;
                }
                costs[next] = cost;
                parents[next] = current.index;
                open.push({next, cost, cost + heuristic(to)});
            }
        }
    }
    return false;
}
