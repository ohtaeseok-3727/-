#include "stdafx.h"
#include "EnemyStateMachine.h"
#include "EnemySystem.h"
#include "EnemyPatrol.h"
#include <algorithm>
#include <cmath>

void EnemyStateMachine::Transition(FieldEnemy& enemy, EnemyState state) const
{
    if (enemy.ai.state == state)
    {
        return;
    }
    enemy.ai.state = state;
    enemy.stateTime = 0.f;
    // A changed state has a changed destination (especially chase -> return).
    // Same-state updates return above and retain their cached path.
    enemy.ai.path.clear();
    enemy.ai.waypoint = 0;
    enemy.ai.repathTimer = 0.f;
    if (state != EnemyState::Windup && state != EnemyState::Heal)
    {
        enemy.windup = 0.f;
    }
}

void EnemyStateMachine::Interrupt(FieldEnemy& enemy) const
{
    Transition(enemy, enemy.health <= 0 ? EnemyState::Dead : EnemyState::Recovery);
    enemy.windup = 0.f;
}

void EnemyStateMachine::Chase(
    FieldEnemy& enemy, float dt, WorldPosition player, const LevelMap& map, int& searchBudget) const
{
    auto& ai = enemy.ai;
    if (ai.mapRevision != map.Revision())
    {
        ai.path.clear();
        ai.waypoint = 0;
        ai.repathTimer = 0.f;
        ai.mapRevision = map.Revision();
    }
    ai.repathTimer = (std::max)(0.f, ai.repathTimer - dt);
    if (ai.repathTimer <= 0.f && searchBudget > 0)
    {
        --searchBudget;
        m_Pathfinder.FindPath(enemy.position, player, map, ai.path);
        ai.waypoint = 0;
        // Failed searches are also throttled; never fall back to walking through a wall.
        ai.repathTimer = ai.path.empty() ? .8f : enemy.ai.state == EnemyState::Return ? 2.f : .45f;
    }

    const float speed = enemy.legInjured ? enemy.stats.injuredSpeed : enemy.stats.speed;
    float remaining = speed * dt;
    while (remaining > 0.f && ai.waypoint < ai.path.size())
    {
        const auto target = ai.path[ai.waypoint];
        const float dx = target.x - enemy.position.x;
        const float dy = target.y - enemy.position.y;
        const float distance = std::sqrt(dx * dx + dy * dy);
        if (distance <= .001f)
        {
            ++ai.waypoint;
            continue;
        }
        const float step = (std::min)(remaining, distance);
        const WorldPosition next{enemy.position.x + dx / distance * step,
                                 enemy.position.y + dy / distance * step};
        if (!map.ClearPath(enemy.position, next))
        {
            ai.path.clear();
            ai.waypoint = 0;
            // Keep the retry timer so blocked movement cannot request A* every frame.
            break;
        }
        enemy.position = next;
        if (std::abs(dx) > .001f)
        {
            enemy.faceLeft = dx < 0.f;
        }
        remaining -= step;
        if (step >= distance)
        {
            ++ai.waypoint;
        }
    }
}

int EnemyStateMachine::Update(FieldEnemy& enemy,
                              std::vector<FieldEnemy>& enemies,
                              float dt,
                              WorldPosition player,
                              const LevelMap& map,
                              bool supportEnabled,
                              int& searchBudget) const
{
    enemy.animationTime += dt;
    enemy.stateTime += dt;
    enemy.hitFlash = (std::max)(0.f, enemy.hitFlash - dt);
    if (enemy.health <= 0)
    {
        Transition(enemy, EnemyState::Dead);
        return 0;
    }
    if (enemy.stats.scriptedAI)
    {
        return 0; // Chapter-owned actions use the same damage/loot/render entity.
    }
    enemy.cooldown = (std::max)(0.f, enemy.cooldown - dt);
    const float dx = player.x - enemy.position.x;
    const float dy = player.y - enemy.position.y;
    const float distanceSquared = dx * dx + dy * dy;
    if (!supportEnabled && (enemy.role == EnemyRole::Healer || enemy.role == EnemyRole::Ranged))
    {
        Transition(enemy, EnemyState::Disabled);
        return 0;
    }

    // Resolve committed actions before selecting a new state.
    switch (enemy.ai.state)
    {
    case EnemyState::Windup:
    {
        enemy.windup = (std::max)(0.f, enemy.windup - dt);
        if (enemy.windup > 0.f)
        {
            return 0;
        }
        const float range = enemy.AttackRange();
        const bool hit = distanceSquared < range * range && map.ClearPath(enemy.position, player);
        enemy.cooldown = enemy.role == EnemyRole::Ranged ? 2.4f : 1.4f;
        Transition(enemy, EnemyState::Recovery);
        return hit ? enemy.stats.damage : 0;
    }
    case EnemyState::Heal:
        enemy.windup = (std::max)(0.f, enemy.windup - dt);
        if (enemy.windup > 0.f)
        {
            return 0;
        }
        Transition(enemy, EnemyState::Recovery);
        break;
    default:
        break;
    }

    if (enemy.role == EnemyRole::Healer)
    {
        Transition(enemy, enemy.cooldown > 0.f ? EnemyState::Recovery : EnemyState::Idle);
        if (enemy.cooldown > 0.f)
        {
            return 0;
        }
        FieldEnemy* target = nullptr;
        float best = 340.f * 340.f;
        for (auto& candidate : enemies)
        {
            if (&candidate == &enemy || candidate.health <= 0 ||
                candidate.health >= candidate.maxHealth)
            {
                continue;
            }
            const float tx = candidate.position.x - enemy.position.x;
            const float ty = candidate.position.y - enemy.position.y;
            const float squared = tx * tx + ty * ty;
            if (squared < best && map.ClearPath(enemy.position, candidate.position))
            {
                target = &candidate;
                best = squared;
            }
        }
        if (target)
        {
            target->health = (std::min)(target->maxHealth, target->health + 18);
            Transition(enemy, EnemyState::Heal);
            enemy.cooldown = 3.f;
            enemy.windup = .5f;
        }
        return 0;
    }

    if (enemy.role != EnemyRole::Boss)
    {
        if (!EnemyPatrol::Detect(enemy, player, dt, map))
        {
            if (enemy.ai.returning)
            {
                if (EnemyPatrol::DistanceSquared(enemy.position, enemy.ai.home) <= 16.f)
                {
                    enemy.ai.returning = false;
                    enemy.ai.patrolTarget = enemy.position;
                    Transition(enemy, EnemyState::Patrol);
                }
                else
                {
                    Transition(enemy, EnemyState::Return);
                    Chase(enemy, dt, enemy.ai.home, map, searchBudget);
                }
            }
            else
            {
                Transition(enemy, EnemyState::Patrol);
                EnemyPatrol::Update(enemy, dt, map);
            }
            return 0;
        }
    }
    else if (distanceSquared >= enemy.stats.detectionRange * enemy.stats.detectionRange)
    {
        Transition(enemy, EnemyState::Idle);
        return 0;
    }
    enemy.faceLeft = dx < 0.f;
    const float range = enemy.AttackRange();
    const bool canAttack =
        distanceSquared < (range - 8.f) * (range - 8.f) && map.ClearPath(enemy.position, player);
    if (canAttack)
    {
        if (enemy.cooldown <= 0.f)
        {
            Transition(enemy, EnemyState::Windup);
            enemy.windup = enemy.AttackWindupDuration();
        }
        else
        {
            Transition(enemy, EnemyState::Recovery);
        }
    }
    else
    {
        Transition(enemy, EnemyState::Chase);
        Chase(enemy,
              dt,
              enemy.role == EnemyRole::Boss ? player : enemy.ai.lastSeen,
              map,
              searchBudget);
    }
    return 0;
}
