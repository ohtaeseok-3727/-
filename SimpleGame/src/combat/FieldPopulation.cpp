#include "stdafx.h"
#include "FieldPopulation.h"
#include <cmath>

namespace
{
    float DistanceSquared(WorldPosition a, WorldPosition b)
    {
        const float dx = a.x - b.x;
        const float dy = a.y - b.y;
        return dx * dx + dy * dy;
    }
}

void FieldPopulation::Populate(const LevelMap& map, EnemySystem& enemies)
{
    m_Packs.clear();
    m_Elapsed = 0.f;
    const auto& layout = map.Layout();
    const auto safe = [&](WorldPosition p)
    {
        if (layout.Interior(p) || layout.RegionAt(p) == MapRegion::Village ||
            DistanceSquared(p, layout.Definition().entry) < 300.f * 300.f || map.Blocked(p.x, p.y))
        {
            return false;
        }
        for (const auto& site : layout.Landmarks())
        {
            if (DistanceSquared(p, site.position) < 120.f * 120.f)
            {
                return false;
            }
        }
        for (const auto center : layout.EncounterCenters())
        {
            if (DistanceSquared(p, center) < 100.f * 100.f)
            {
                return false;
            }
        }
        return true;
    };
    // Sample the stage routes rather than filling the outskirts of the first area.
    for (const auto& road : layout.Roads())
    {
        const float dx = road.to.x - road.from.x;
        const float dy = road.to.y - road.from.y;
        const float length = std::hypot(dx, dy);
        if (length < 1.f)
        {
            continue;
        }
        const int samples = static_cast<int>(std::ceil(length / 240.f));
        for (int sample = 0; sample < samples; ++sample)
        {
            if (m_Packs.size() >= 12)
            {
                return;
            }
            const float t = (sample + .5f) / samples;
            for (const float offset : {0.f, -48.f, 48.f, -96.f, 96.f})
            {
                const WorldPosition center{road.from.x + dx * t - dy / length * offset,
                                           road.from.y + dy * t + dx / length * offset};
                Pack pack{{center,
                           {center.x - dy / length * 32.f, center.y + dx / length * 32.f},
                           {center.x + dy / length * 32.f, center.y - dx / length * 32.f}},
                          100000 + m_Packs.size()};
                bool valid = true;
                pack.health = layout.Definition().fieldEnemyHealth;
                for (const auto& existing : m_Packs)
                {
                    valid =
                        valid && DistanceSquared(existing.positions[0], center) >= 280.f * 280.f;
                }
                for (const auto p : pack.positions)
                {
                    valid = valid && safe(p);
                }
                if (valid)
                {
                    m_Packs.push_back(pack);
                    Spawn(pack, enemies);
                    break;
                }
            }
        }
    }
}

void FieldPopulation::Spawn(const Pack& pack, EnemySystem& enemies) const
{
    EnemyStats stats;
    stats.health = pack.health;
    stats.detectionRange = 200.f;
    for (const auto p : pack.positions)
    {
        enemies.Add(p, pack.encounter, EnemyRole::Melee, stats);
    }
}

void FieldPopulation::Update(float dt, WorldPosition player, EnemySystem& enemies)
{
    m_Elapsed += dt;
    if (m_Elapsed < 1.f)
    {
        return;
    }
    const float elapsed = m_Elapsed;
    m_Elapsed = 0.f;
    // One scan per second instead of one full enemy scan for every pack.
    std::array<bool, 12> alive{};
    for (const auto& enemy : enemies.Enemies())
    {
        if (enemy.health > 0 && enemy.encounter >= 100000 &&
            enemy.encounter - 100000 < m_Packs.size())
        {
            alive[enemy.encounter - 100000] = true;
        }
    }
    for (auto& pack : m_Packs)
    {
        if (alive[pack.encounter - 100000])
        {
            pack.cooldown = 45.f;
            continue;
        }
        pack.cooldown -= elapsed;
        // Never replace a defeated pack next to the player; far regions stay dormant.
        const float distance = DistanceSquared(player, pack.positions[0]);
        if (pack.cooldown <= 0.f && distance > 900.f * 900.f && distance < 1500.f * 1500.f)
        {
            Spawn(pack, enemies);
            pack.cooldown = 45.f;
        }
    }
}
