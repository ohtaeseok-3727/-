#pragma once
#include "EnemySystem.h"
#include <algorithm>
#include <array>
#include <cmath>

// Local patrol and perception policy; combat and A* remain in the state machine.
class EnemyPatrol
{
  public:
    static float DistanceSquared(WorldPosition a, WorldPosition b)
    {
        const float dx = a.x - b.x;
        const float dy = a.y - b.y;
        return dx * dx + dy * dy;
    }

    static bool Detect(FieldEnemy& enemy, WorldPosition player, float dt, const LevelMap& map)
    {
        auto& ai = enemy.ai;
        if (ai.returning)
        {
            return false;
        }
        if (DistanceSquared(enemy.position, ai.home) > 480.f * 480.f)
        {
            ai.alerted = false;
            ai.returning = true;
            return false;
        }
        ai.lostSightTime += dt;
        ai.perceptionTimer -= dt;
        if (ai.perceptionTimer <= 0.f)
        {
            ai.perceptionTimer = .2f;
            const float range = enemy.stats.detectionRange + (ai.alerted ? 80.f : 0.f);
            if (DistanceSquared(enemy.position, player) < range * range &&
                !map.IsTown(player.x, player.y) && map.ClearPath(enemy.position, player))
            {
                ai.alerted = true;
                ai.lastSeen = player;
                ai.lostSightTime = 0.f;
            }
        }
        if (ai.alerted && ai.lostSightTime > 1.f)
        {
            ai.alerted = false;
            ai.returning = true;
        }
        return ai.alerted;
    }

    static void Update(FieldEnemy& enemy, float dt, const LevelMap& map)
    {
        auto& ai = enemy.ai;
        ai.patrolWait -= dt;
        if (ai.patrolWait > 0.f)
        {
            return;
        }
        if (DistanceSquared(enemy.position, ai.patrolTarget) < 16.f)
        {
            static constexpr std::array<WorldPosition, 8> directions = {WorldPosition{1, 0},
                                                                        {0, 1},
                                                                        {-1, 0},
                                                                        {0, -1},
                                                                        {.7f, .7f},
                                                                        {-.7f, .7f},
                                                                        {-.7f, -.7f},
                                                                        {.7f, -.7f}};
            const auto direction = directions[ai.patrolStep++ % directions.size()];
            const WorldPosition target{ai.home.x + direction.x * 80.f,
                                       ai.home.y + direction.y * 80.f};
            ai.patrolWait = .6f + static_cast<float>(enemy.id % 5) * .12f;
            if (!map.IsTown(target.x, target.y) && map.ClearPath(enemy.position, target))
            {
                ai.patrolTarget = target;
            }
            return;
        }
        const float dx = ai.patrolTarget.x - enemy.position.x;
        const float dy = ai.patrolTarget.y - enemy.position.y;
        const float distance = std::hypot(dx, dy);
        const float step = (std::min)(distance, enemy.stats.speed * .45f * dt);
        const WorldPosition next{enemy.position.x + dx / distance * step,
                                 enemy.position.y + dy / distance * step};
        if (!map.ClearPath(enemy.position, next))
        {
            ai.patrolTarget = enemy.position;
            ai.patrolWait = .8f;
            return;
        }
        enemy.position = next;
        if (std::abs(dx) > .001f)
        {
            enemy.faceLeft = dx < 0.f;
        }
    }
};
