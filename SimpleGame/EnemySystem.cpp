#include "stdafx.h"
#include "EnemySystem.h"
#include <algorithm>
#include <cmath>

void EnemySystem::Reset(unsigned seed, const LevelMap& map)
{
    m_Random.seed(seed ^ 0x329ac17u);
    const auto& centers = map.Layout().EncounterCenters();
    m_Enemies.assign(centers.size(), FieldEnemy{});
    for (std::size_t i = 0; i < m_Enemies.size(); ++i)
    {
        m_Enemies[i].encounter = i;
        Spawn(m_Enemies[i], map);
    }
    m_Enemies.front().position = centers.front();
}

void EnemySystem::Spawn(FieldEnemy& enemy, const LevelMap& map)
{
    const auto encounter = enemy.encounter;
    const auto center = map.Layout().EncounterCenters()[encounter];
    std::uniform_real_distribution<float> angle(0.f, 6.2831853f);
    std::uniform_real_distribution<float> radius(20.f, 75.f);
    for (int attempt = 0; attempt < 40; ++attempt)
    {
        const float a = angle(m_Random);
        const float r = radius(m_Random);
        WorldPosition p{center.x + std::cos(a) * r, center.y + std::sin(a) * r};
        if (!map.IsTown(p.x, p.y) && !map.Blocked(p.x, p.y))
        {
            enemy = FieldEnemy{};
            enemy.encounter = encounter;
            enemy.position = p;
            return;
        }
    }
    // Every authored encounter center is a reserved, walkable clearing.
    enemy = FieldEnemy{};
    enemy.encounter = encounter;
    enemy.position = center;
}

int EnemySystem::Update(float dt, WorldPosition player, const LevelMap& map)
{
    int incomingDamage = 0;
    for (auto& enemy : m_Enemies)
    {
        enemy.hitFlash = (std::max)(0.f, enemy.hitFlash - dt);
        if (enemy.health <= 0)
        {
            enemy.respawn -= dt;
            if (enemy.respawn <= 0.f)
            {
                const float dx = player.x - enemy.position.x;
                const float dy = player.y - enemy.position.y;
                const auto center = map.Layout().EncounterCenters()[enemy.encounter];
                const float cx = player.x - center.x;
                const float cy = player.y - center.y;
                if (dx * dx + dy * dy > 220.f * 220.f && cx * cx + cy * cy > 320.f * 320.f)
                {
                    Spawn(enemy, map);
                }
            }
            continue;
        }

        float dx = player.x - enemy.position.x;
        float dy = player.y - enemy.position.y;
        const float distance = std::sqrt(dx * dx + dy * dy);
        if (distance > 1500.f)
        {
            continue;
        }
        enemy.cooldown = (std::max)(0.f, enemy.cooldown - dt);
        if (map.IsTown(player.x, player.y))
        {
            enemy.windup = 0.f;
            continue;
        }
        if (enemy.windup > 0.f)
        {
            enemy.windup -= dt;
            if (enemy.windup <= 0.f)
            {
                if (distance < 55.f && map.ClearPath(enemy.position, player))
                {
                    // Global player invulnerability prevents a crowd's simultaneous burst.
                    incomingDamage = LevelTuning::EnemyDamage;
                }
                enemy.cooldown = 1.2f;
            }
            continue;
        }
        if (distance < 46.f && enemy.cooldown <= 0.f && map.ClearPath(enemy.position, player))
        {
            enemy.windup = .6f;
        }
        else if (distance > 38.f && distance < 280.f)
        {
            dx /= distance;
            dy /= distance;
            const float step = 65.f * dt;
            const auto tryMove = [&](float vx, float vy)
            {
                if (std::abs(vx) + std::abs(vy) < .0001f)
                {
                    return false;
                }
                WorldPosition next{enemy.position.x + vx * step, enemy.position.y + vy * step};
                if (!map.IsTown(next.x, next.y) && map.ClearPath(enemy.position, next))
                {
                    enemy.position = next;
                    return true;
                }
                return false;
            };
            if (!tryMove(dx, dy) && !tryMove(dx, 0.f) && !tryMove(0.f, dy))
            {
                if (!tryMove(-dy, dx))
                {
                    tryMove(dy, -dx);
                }
            }
        }
    }
    return incomingDamage;
}

std::vector<WorldPosition> EnemySystem::Strike(WorldPosition player,
                                               bool faceLeft,
                                               int damage,
                                               const LevelMap& map)
{
    std::vector<WorldPosition> defeated;
    for (auto& enemy : m_Enemies)
    {
        const float dx = (enemy.position.x - player.x) * (faceLeft ? -1.f : 1.f);
        const float dy = enemy.position.y - player.y;
        if (enemy.health <= 0 || dx < -18.f ||
            dx * dx + dy * dy * 2.f > LevelTuning::HitRange * LevelTuning::HitRange ||
            !map.ClearPath(player, enemy.position))
        {
            continue;
        }
        enemy.health = (std::max)(0, enemy.health - damage);
        enemy.hitFlash = .18f;
        enemy.windup = 0.f;
        enemy.cooldown = .8f;
        if (enemy.health == 0)
        {
            enemy.respawn = LevelTuning::RespawnSeconds;
            defeated.push_back(enemy.position);
        }
    }
    return defeated;
}

const std::vector<FieldEnemy>& EnemySystem::Enemies() const
{
    return m_Enemies;
}
