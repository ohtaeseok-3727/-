#include "stdafx.h"
#include "CombatCueView.h"
#include "SceneCamera.h"
#include <algorithm>
#include <cmath>

void CombatCueView::DrawArea(Renderer& renderer, Point center, float radius, float progress)
{
    progress = std::clamp(progress, 0.f, 1.f);
    const float depth = SceneCamera::GroundDepthScale();
    const Color color(1.f, .64f - .42f * progress, .12f, .8f);
    renderer.Ellipse(center.x,
                     center.y,
                     radius,
                     radius * depth,
                     Color(color.r, color.g, color.b, .10f + .09f * progress));
    constexpr int segments = 40;
    for (int segment = 0; segment < segments; ++segment)
    {
        const float a = segment * 6.2831853f / segments;
        const float b = (segment + 1) * 6.2831853f / segments;
        const float inner = radius - 2.f;
        renderer.Quad({center.x + std::cos(a) * radius, center.y + std::sin(a) * radius * depth},
                      {center.x + std::cos(b) * radius, center.y + std::sin(b) * radius * depth},
                      {center.x + std::cos(b) * inner, center.y + std::sin(b) * inner * depth},
                      {center.x + std::cos(a) * inner, center.y + std::sin(a) * inner * depth},
                      color);
    }
}

void CombatCueView::DrawEnemy(Renderer& renderer, const FieldEnemy& enemy, Point foot, float height)
{
    if (enemy.health <= 0 || enemy.stats.scriptedAI || enemy.ai.state != EnemyState::Windup)
    {
        return;
    }
    const float progress = std::clamp(1.f - enemy.windup / enemy.AttackWindupDuration(), 0.f, 1.f);
    DrawArea(renderer, foot, enemy.AttackRange(), progress);
    renderer.Rect(foot.x - 22.f, foot.y - height - 15.f, 44.f, 4.f, Color(.14f, .09f, .06f));
    renderer.Rect(foot.x - 22.f,
                  foot.y - height - 15.f,
                  44.f * progress,
                  4.f,
                  Color(1.f, .64f - .42f * progress, .12f));
    renderer.Text(foot.x - 16.f,
                  foot.y - height - 23.f,
                  enemy.role == EnemyRole::Ranged ? "사격!" : "공격!",
                  Color(1.f, .8f, .4f));
}
