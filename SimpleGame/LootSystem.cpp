#include "stdafx.h"
#include "LootSystem.h"
#include <algorithm>
#include <cmath>

void LootSystem::Reset(unsigned seed)
{
    m_Random.seed(seed ^ 0x839271u);
    m_Drops.clear();
}

void LootSystem::Drop(WorldPosition position, int killCount, int level)
{
    std::uniform_int_distribution<int> roll(1, 100);
    std::uniform_int_distribution<int> coins(3, 8);
    std::uniform_int_distribution<int> quality(2, 6);
    m_Drops.push_back({position, LootKind::Gold, coins(m_Random)});
    // Early guarantees teach each reward; later kills use independent probabilities.
    if (killCount == 2 || roll(m_Random) <= 25)
    {
        m_Drops.push_back({{position.x - 12.f, position.y},
                           LootKind::Equipment,
                           quality(m_Random) + (std::min)(level - 1, 10)});
    }
    if (killCount == 1 || roll(m_Random) <= 35)
    {
        m_Drops.push_back({{position.x + 12.f, position.y}, LootKind::Health, 25});
    }
}

std::vector<LootDrop> LootSystem::Update(float dt, WorldPosition player)
{
    std::vector<LootDrop> collected;
    for (auto it = m_Drops.begin(); it != m_Drops.end();)
    {
        it->age += dt;
        float dx = player.x - it->position.x;
        float dy = player.y - it->position.y;
        const float distance = std::sqrt(dx * dx + dy * dy);
        if (distance <= LevelTuning::MagnetRadius)
        {
            it->attracted = true;
        }
        if (it->attracted)
        {
            const float step = (240.f + 2.f * distance) * dt;
            if (distance <= LevelTuning::PickupRadius || step >= distance)
            {
                collected.push_back(*it);
                it = m_Drops.erase(it);
                continue;
            }
            it->position.x += dx / distance * step;
            it->position.y += dy / distance * step;
        }
        // Bound abandoned world drops while keeping all nearby rewards available.
        if (!it->attracted && it->age > 120.f)
        {
            it = m_Drops.erase(it);
        }
        else
        {
            ++it;
        }
    }
    return collected;
}

const std::vector<LootDrop>& LootSystem::Drops() const
{
    return m_Drops;
}
