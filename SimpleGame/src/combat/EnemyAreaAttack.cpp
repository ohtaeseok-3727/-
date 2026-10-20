#include "stdafx.h"
#include "EnemySystem.h"
#include <algorithm>
#include <cmath>

std::vector<WorldPosition> EnemySystem::StrikeArea(WorldPosition center, float radius, int damage)
{
    std::vector<WorldPosition> defeated;
    if (!std::isfinite(radius) || radius <= 0.f || damage <= 0)
        return defeated;
    const float radiusSquared = radius * radius;
    for (auto& enemy : m_Enemies)
    {
        const float dx = enemy.position.x - center.x, dy = enemy.position.y - center.y;
        if (enemy.health <= 0 || dx * dx + dy * dy > radiusSquared)
            continue;
        // Ground circle membership is sufficient: no sword, facing or sight-line test.
        enemy.health = (std::max)(enemy.stats.minimumHealth, enemy.health - damage);
        enemy.hitFlash = .18f;
        if (!enemy.stats.scriptedAI)
        {
            m_StateMachine.Interrupt(enemy);
            enemy.cooldown = .8f;
        }
        if (enemy.health == 0)
            defeated.push_back(enemy.position);
    }
    return defeated;
}
