#include "stdafx.h"
#include "MageUltimate.h"
#include "GameSession.h"
#include "SkillSystem.h"
#include <algorithm>
#include <cmath>

void MageUltimate::Reset()
{
    m_Time = Duration;
    m_Pulses = 0;
    m_Exploded = false;
}

void MageUltimate::Cast(const SkillUseRequest& request, WorldPosition player, bool faceLeft)
{
    if (request.skillId != "mage_sword_song")
        return;
    m_Center = player;
    m_FaceLeft = faceLeft;
    m_Time = 0;
    m_Pulses = 0;
    m_Exploded = false;
    m_Damage = static_cast<int>(std::lround(request.calculatedDamage));
    m_BurstDamage = static_cast<int>(std::lround(request.calculatedBurstDamage));
}

void MageUltimate::Update(float dt, GameSession& session)
{
    if (!Active() || !std::isfinite(dt) || dt <= 0.f)
        return;
    if (session.IsDefeated() || session.CurrentChapter().ControlLocked())
    {
        Reset();
        return;
    }
    m_Time = (std::min)(Duration, m_Time + static_cast<double>(dt));
    // Integer event cursor handles crossed boundaries without skipped or repeated ticks.
    while (m_Pulses < PulseCount && m_Time + 1.e-7 >= PulseTime(m_Pulses))
    {
        session.StrikeArea(m_Center, Radius, m_Damage);
        ++m_Pulses;
    }
    if (!m_Exploded && m_Time + 1.e-7 >= ExplosionTime)
    {
        session.StrikeArea(m_Center, Radius, m_BurstDamage);
        m_Exploded = true;
    }
}
