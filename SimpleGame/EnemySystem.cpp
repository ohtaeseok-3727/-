#include "stdafx.h"
#include "EnemySystem.h"
#include <algorithm>
#include <cmath>

void EnemySystem::Reset(unsigned seed, const LevelMap& map)
{
    m_Random.seed(seed ^ 0x329ac17u);
    m_Enemies.assign(LevelTuning::EnemyCount, FieldEnemy{});
    for (auto& enemy : m_Enemies)
    {
        Spawn(enemy, {0.f, 0.f}, map);
    }
    // A visible first target on the village's east road teaches the farming loop.
    m_Enemies.front().position = {340.f, 0.f};
}

void EnemySystem::Spawn(FieldEnemy& enemy, WorldPosition player, const LevelMap& map)
{
    std::uniform_real_distribution<float> angle(0.f, 6.2831853f);
    std::uniform_real_distribution<float> radius(320.f, 780.f);
    for (int attempt = 0; attempt < 40; ++attempt)
    {
        const float a = angle(m_Random);
        const float r = radius(m_Random);
        WorldPosition p{player.x + std::cos(a) * r, player.y + std::sin(a) * r};
        if (!map.IsTown(p.x, p.y) && !map.Blocked(p.x, p.y))
        {
            enemy = FieldEnemy{};
            enemy.position = p;
            return;
        }
    }
    // The shared road lattice is always clear, so retries never leave an invalid spawn.
    enemy = FieldEnemy{};
    enemy.position = {std::round(player.x / 512.f) * 512.f + 512.f,
                      std::round(player.y / 512.f) * 512.f};
    if (map.IsTown(enemy.position.x, enemy.position.y))
    {
        enemy.position.x += 512.f;
    }
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
                Spawn(enemy, player, map);
            }
            continue;
        }

        float dx = player.x - enemy.position.x;
        float dy = player.y - enemy.position.y;
        const float distance = std::sqrt(dx * dx + dy * dy);
        if (distance > 1500.f)
        {
            Spawn(enemy, player, map);
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
        else if (distance > 38.f && distance < 360.f)
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
