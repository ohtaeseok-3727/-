#include "stdafx.h"
#include "BasicAttack.h"
#include "CombatVisualData.h"
#include <cmath>
#include <cstring>

BasicAttack::BasicAttack()
{
    const std::array<std::array<const char*, 3>, 2> ids = {
        {{{"mage-1", "mage-2", "mage-3"}}, {{"knight-1", "knight-2", "knight-3"}}}};

    for (std::size_t profession = 0; profession < ids.size(); ++profession)
    {
        for (std::size_t step = 0; step < ids[profession].size(); ++step)
        {
            for (const auto& clip : CombatVisuals::clips)
            {
                if (std::strcmp(clip.id, ids[profession][step]) == 0)
                {
                    m_Clips[profession][step].id = clip.id;
                    m_Clips[profession][step].duration = clip.duration;
                    m_Clips[profession][step].hitTime = clip.effectStart;
                    break;
                }
            }
        }
    }
}

void BasicAttack::Reset()
{
    m_Step = 0;
    m_BufferedPresses = 0;
    m_ElapsedMilliseconds = 0.f;
    m_HitEmitted = false;
    m_PendingHits.clear();
}

void BasicAttack::Press(int profession)
{
    if (profession < 0 || profession >= static_cast<int>(m_Clips.size()))
    {
        return;
    }

    if (m_Profession != profession)
    {
        Reset();
        m_Profession = profession;
    }

    if (m_Step == 0)
    {
        Begin(1);
    }
    else if (IsAttacking())
    {
        // Keep rapid 1-2-3 presses, but never queue beyond the third strike.
        if (m_Step + m_BufferedPresses < 3)
        {
            ++m_BufferedPresses;
        }
    }
    else
    {
        Begin(m_Step + 1);
    }
}

void BasicAttack::Update(float seconds)
{
    if (m_Step == 0 || !std::isfinite(seconds) || seconds <= 0.f)
    {
        return;
    }

    m_ElapsedMilliseconds += seconds * 1000.f;

    while (m_Step != 0)
    {
        if (!m_HitEmitted && m_ElapsedMilliseconds >= CurrentClip().hitTime)
        {
            m_PendingHits.push_back(m_Step);
            m_HitEmitted = true;
        }
        if (m_ElapsedMilliseconds < CurrentClip().duration)
        {
            break;
        }
        if (m_Step == 3)
        {
            // Natural completion retains hit events until the world consumes them.
            m_Step = 0;
            m_BufferedPresses = 0;
            m_ElapsedMilliseconds = 0.f;
        }
        else if (m_BufferedPresses > 0)
        {
            const float remainder = m_ElapsedMilliseconds - CurrentClip().duration;
            --m_BufferedPresses;
            Begin(m_Step + 1);
            m_ElapsedMilliseconds = remainder;
        }
        else
        {
            if (m_ElapsedMilliseconds >= CurrentClip().duration + m_ComboGraceMilliseconds)
            {
                m_Step = 0;
                m_ElapsedMilliseconds = 0.f;
            }

            break;
        }
    }
}

bool BasicAttack::IsAttacking() const
{
    return m_Step != 0 && m_ElapsedMilliseconds < CurrentClip().duration;
}

const char* BasicAttack::ClipId() const
{
    return m_Step == 0 ? nullptr : CurrentClip().id;
}

float BasicAttack::ElapsedMilliseconds() const
{
    return m_ElapsedMilliseconds;
}

int BasicAttack::Step() const
{
    return m_Step;
}

std::vector<int> BasicAttack::ConsumeHits()
{
    std::vector<int> hits;
    hits.swap(m_PendingHits);
    return hits;
}

void BasicAttack::Begin(int step)
{
    if (step < 1 || step > 3 || !m_Clips[m_Profession][step - 1].id ||
        m_Clips[m_Profession][step - 1].duration <= 0.f)
    {
        Reset();
        return;
    }

    m_Step = step;
    m_ElapsedMilliseconds = 0.f;
    m_HitEmitted = false;
}

const BasicAttack::AttackClip& BasicAttack::CurrentClip() const
{
    return m_Clips[m_Profession][m_Step - 1];
}
