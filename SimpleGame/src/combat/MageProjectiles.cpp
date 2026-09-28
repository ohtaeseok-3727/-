#include "stdafx.h"
#include "MageProjectiles.h"
#include "GameSession.h"
#include "SceneCamera.h"
#include "SkillSystem.h"
#include <algorithm>
#include <cmath>
#include <utility>

bool MageProjectile::Visible() const
{
    return kind == MageProjectileKind::Sword || age >= delay;
}

void MageProjectiles::Reset()
{
    m_Projectiles.clear();
    m_CastTime = 1.f;
}

void MageProjectiles::Cast(const SkillUseRequest& request, WorldPosition player, bool faceLeft)
{
    if (request.skillId != "mage_wind_blades" && request.skillId != "mage_mana_swords")
    {
        return;
    }
    m_CastTime = 0.f;
    m_FaceLeft = faceLeft;
    const bool wind = request.skillId == "mage_wind_blades";
    const float direction = faceLeft ? -1.f : 1.f;
    const float swordX[] = {30.f, -8.f, 60.f, 15.f, 75.f};
    const float swordY[] = {-85.f, -134.f, -30.f, -54.f, -110.f};
    const int count = (std::min)(request.projectileCount, wind ? 3 : 5);
    for (int index = 0; index < count; ++index)
    {
        MageProjectile shot;
        shot.kind = wind ? MageProjectileKind::Wind : MageProjectileKind::Sword;
        shot.variant = index;
        shot.faceLeft = faceLeft;
        shot.damage = static_cast<int>(std::lround(request.calculatedDamage));
        shot.delay = .340f + index * (wind ? .120f : .080f);
        // Wind uses the preview's 900px viewport as a fixed world-distance reference.
        shot.range = wind ? 861.2f : 500.f;
        shot.speed = wind ? shot.range / (1.6f / 3.f) : 1200.f;
        shot.origin = player;
        const float swordScale = PlayerScale / .36f;
        shot.origin.x +=
            direction * (wind ? 70.f * PlayerScale + 20.f : swordX[index] * swordScale);
        if (!wind)
        {
            shot.origin.y +=
                (swordY[index] * swordScale + Height) / SceneCamera::GroundDepthScale();
        }
        shot.position = shot.origin;
        m_Projectiles.push_back(std::move(shot));
    }
}

void MageProjectiles::Update(float dt, GameSession& level, const LevelMap& map)
{
    if (!std::isfinite(dt) || dt <= 0.f)
    {
        return;
    }
    m_CastTime += dt;
    for (auto& shot : m_Projectiles)
    {
        shot.age += dt;
        if (shot.age < shot.delay)
        {
            continue;
        }
        const float nextDistance = (std::min)(shot.range, (shot.age - shot.delay) * shot.speed);
        const WorldPosition previous = shot.position;
        const float targetX = shot.origin.x + (shot.faceLeft ? -nextDistance : nextDistance);
        const int steps =
            (std::max)(1, static_cast<int>(std::ceil(std::abs(targetX - previous.x) / 4.f)));
        bool blocked = false;
        for (int step = 1; step <= steps; ++step)
        {
            const float x = previous.x + (targetX - previous.x) * step / steps;
            if (map.Blocked(x, shot.position.y))
            {
                blocked = true;
                break;
            }
            shot.position.x = x;
        }
        shot.distance = blocked ? shot.range : nextDistance;
        // Sweep the complete step: fast shots cannot skip enemies between frames.
        const float collisionScale =
            PlayerScale / (shot.kind == MageProjectileKind::Sword ? .36f : .35f);
        const float radiusY = collisionScale * (shot.kind == MageProjectileKind::Sword ? 12.f
                                                : shot.variant == 0                    ? 69.f
                                                : shot.variant == 1                    ? 62.f
                                                                                       : 35.f);
        const float radiusX =
            collisionScale * (shot.kind == MageProjectileKind::Sword ? 40.f : 83.f);
        if (!blocked || shot.position.x != previous.x)
        {
            level.StrikeProjectile(
                previous, shot.position, radiusX, radiusY, shot.damage, shot.hitTargets, map);
        }
    }
    m_Projectiles.erase(std::remove_if(m_Projectiles.begin(),
                                       m_Projectiles.end(),
                                       [](const MageProjectile& shot)
                                       {
                                           return shot.age >= shot.delay &&
                                                  shot.distance >= shot.range;
                                       }),
                        m_Projectiles.end());
}

bool MageProjectiles::Casting() const
{
    return m_CastTime < .755f;
}

float MageProjectiles::CastMilliseconds() const
{
    return m_CastTime * 1000.f;
}

bool MageProjectiles::CastFaceLeft() const
{
    return m_FaceLeft;
}

const std::vector<MageProjectile>& MageProjectiles::Projectiles() const
{
    return m_Projectiles;
}
