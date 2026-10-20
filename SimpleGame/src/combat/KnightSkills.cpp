#include "stdafx.h"
#include "KnightSkills.h"
#include "KnightSkillData.h"
#include "GameSession.h"
#include "SkillSystem.h"
#include "SceneCamera.h"
#include <algorithm>
#include <cmath>
#include <utility>

bool KnightSkillShot::Visible() const
{
    return !blocked && age >= delay && age < delay + Duration();
}

int KnightSkillShot::Phase() const
{
    return kind == KnightSkillKind::Thrust
               ? KnightSkillData::ThrustPhase(age - delay)
               : KnightSkillData::WavePhase((age - delay) * KnightSkillData::WaveFlight /
                                            Duration());
}

float KnightSkillShot::Duration() const
{
    return kind == KnightSkillKind::Thrust     ? KnightSkillData::ThrustEffectDuration
           : kind == KnightSkillKind::Ultimate ? KnightSkillData::UltimateFlight
                                               : KnightSkillData::WaveFlight;
}

KnightEffectBounds KnightSkillShot::Bounds() const
{
    const int phase = kind == KnightSkillKind::Ultimate ? 2 : Phase();
    const auto& frame = kind == KnightSkillKind::Thrust ? KnightSkillData::ThrustEffect[phase]
                                                        : KnightSkillData::WaveEffect[phase];
    const float factor =
        KnightSkillData::PreviewScale * scale * (kind == KnightSkillKind::Thrust ? .43f : .65f);
    KnightEffectBounds bounds{(frame.left - frame.pivotX) * factor,
                              (frame.top - frame.pivotY) * factor - height,
                              (frame.right - frame.pivotX) * factor,
                              (frame.bottom - frame.pivotY) * factor - height};
    if (kind == KnightSkillKind::Ultimate)
    {
        // Anchor the lower edge: enlarge upwards, never below the original wave.
        bounds.top = bounds.bottom - (bounds.bottom - bounds.top) * KnightSkillData::UltimateHeight;
    }
    return bounds;
}

void KnightSkills::Reset()
{
    m_Shots.clear();
    m_Time = 2.f;
    m_UltimateVisualTime = 2.f;
}

bool KnightSkills::Casting() const
{
    return m_Time < (m_Kind == KnightSkillKind::Thrust     ? KnightSkillData::ThrustDuration
                     : m_Kind == KnightSkillKind::Ultimate ? KnightSkillData::UltimateCastDuration
                                                           : KnightSkillData::WaveCastDuration);
}

void KnightSkills::Cast(const SkillUseRequest& request,
                        WorldPosition player,
                        bool faceLeft,
                        float rangeScale)
{
    if (request.skillId != "knight_thrust" && request.skillId != "knight_wave" &&
        request.skillId != "knight_cheondan")
    {
        return;
    }
    m_Kind = request.skillId == "knight_thrust"     ? KnightSkillKind::Thrust
             : request.skillId == "knight_cheondan" ? KnightSkillKind::Ultimate
                                                    : KnightSkillKind::Wave;
    m_Time = 0;
    m_FaceLeft = faceLeft;
    if (m_Kind == KnightSkillKind::Ultimate)
    {
        m_UltimateVisualTime = 0.f;
        m_UltimateVisualFaceLeft = faceLeft;
    }
    const float sign = faceLeft ? -1.f : 1.f;
    const float ratio = KnightSkillData::PreviewScale * rangeScale;
    const int count = m_Kind == KnightSkillKind::Thrust ? 6 : 1;
    for (int i = 0; i < count; ++i)
    {
        KnightSkillShot shot;
        shot.kind = m_Kind;
        shot.damage = static_cast<int>(std::lround(request.calculatedDamage));
        shot.faceLeft = faceLeft;
        shot.scale = rangeScale;
        shot.caster = player;
        shot.origin = player;
        if (m_Kind == KnightSkillKind::Thrust)
        {
            const auto& event = KnightSkillData::Strikes[i];
            shot.delay = event.time;
            shot.origin.x += sign * (343.f * .44f + event.x) * ratio;
            shot.height = (198.f * .44f - event.y) * ratio;
        }
        else
        {
            shot.delay = m_Kind == KnightSkillKind::Ultimate ? KnightSkillData::UltimateRelease
                                                             : KnightSkillData::WaveRelease;
            shot.origin.x += sign * 170.f * ratio;
            shot.height = 88.f * ratio;
        }
        shot.position = shot.origin;
        m_Shots.push_back(std::move(shot));
    }
}

void KnightSkills::Update(float dt, GameSession& level, const LevelMap& map)
{
    if (!std::isfinite(dt) || dt <= 0.f)
    {
        return;
    }
    m_Time += dt;
    m_UltimateVisualTime = (std::min)(2.f, m_UltimateVisualTime + dt);
    // Visit short effect phases even when a frame spans several strike events.
    while (dt > 0.f && !m_Shots.empty())
    {
        const float step = (std::min)(dt, 1.f / 120.f);
        Step(step, level, map);
        dt -= step;
    }
}

void KnightSkills::Step(float dt, GameSession& level, const LevelMap& map)
{
    for (auto& shot : m_Shots)
    {
        const float previousAge = shot.age;
        shot.age += dt;
        const float duration = shot.Duration();
        if (shot.age < shot.delay || shot.blocked)
        {
            continue;
        }
        if (previousAge <= shot.delay && !map.ClearPath(shot.caster, shot.origin))
        {
            shot.blocked = true;
            continue;
        }
        const float sign = shot.faceLeft ? -1.f : 1.f;
        const auto previous = shot.position;
        if (shot.kind != KnightSkillKind::Thrust)
        {
            const float distance =
                KnightSkillData::WaveDistance * shot.scale *
                (shot.kind == KnightSkillKind::Ultimate ? KnightSkillData::UltimateDistance : 1.f) *
                (std::min)(1.f, (shot.age - shot.delay) / duration);
            const float target = shot.origin.x + sign * distance;
            const int steps =
                (std::max)(1, static_cast<int>(std::ceil(std::abs(target - previous.x) / 4.f)));
            for (int i = 1; i <= steps; ++i)
            {
                const float x = previous.x + (target - previous.x) * i / steps;
                if (map.Blocked(x, shot.position.y))
                {
                    shot.blocked = true;
                    break;
                }
                shot.position.x = x;
            }
            shot.distance = distance;
        }
        const auto bounds = shot.Bounds();
        const float centerX = (bounds.left + bounds.right) * .5f * sign;
        const float centerY = (bounds.top + bounds.bottom) * .5f;
        const float radiusX = (bounds.right - bounds.left) * .5f;
        const float radiusY = (bounds.bottom - bounds.top) * .5f;
        if (!map.Blocked(previous.x, previous.y))
        {
            level.StrikeProjectile({previous.x + centerX, previous.y},
                                   {shot.position.x + centerX, shot.position.y},
                                   radiusX,
                                   radiusY,
                                   shot.damage,
                                   shot.hitTargets,
                                   map,
                                   -centerY,
                                   shot.kind == KnightSkillKind::Thrust ? shot.caster : previous);
        }
    }
    m_Shots.erase(std::remove_if(m_Shots.begin(),
                                 m_Shots.end(),
                                 [](const KnightSkillShot& shot)
                                 {
                                     return shot.blocked ||
                                            shot.age >= shot.delay + shot.Duration();
                                 }),
                  m_Shots.end());
}
