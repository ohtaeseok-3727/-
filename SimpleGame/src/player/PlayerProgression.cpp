#include "stdafx.h"
#include "PlayerProgression.h"
#include "KnightSkillData.h"
#include <algorithm>

void PlayerProgression::Reset(int profession)
{
    m_Profession = profession;
    m_Level = 1;
    m_Experience = 0;
    m_Health = MaxHealth();
    m_Mana = MaxMana();
}

void PlayerProgression::DebugSetLevel(int level)
{
    m_Level = (std::max)(1, level);
    m_Experience = 0;
    m_Health = MaxHealth();
    m_Mana = MaxMana();
}

int PlayerProgression::AddExperience(int amount)
{
    if (amount <= 0)
    {
        return 0;
    }
    m_Experience += amount;
    int gained = 0;
    while (m_Experience >= RequiredExperience())
    {
        m_Experience -= RequiredExperience();
        ++m_Level;
        ++gained;
        // Only refill the increase in max HP, rather than erase all battle damage.
        Heal(15);
    }
    return gained;
}

void PlayerProgression::Heal(int amount)
{
    m_Health = (std::min)(MaxHealth(), m_Health + (std::max)(0, amount));
}

void PlayerProgression::Restore()
{
    m_Health = MaxHealth();
    m_Mana = MaxMana();
}

void PlayerProgression::Damage(int amount)
{
    m_Health = (std::max)(0, m_Health - (std::max)(0, amount));
}

int PlayerProgression::RequiredExperience() const
{
    return 50 + (m_Level - 1) * 25;
}

int PlayerProgression::Level() const
{
    return m_Level;
}

int PlayerProgression::Experience() const
{
    return m_Experience;
}

int PlayerProgression::Health() const
{
    return m_Health;
}

int PlayerProgression::MaxHealth() const
{
    return 100 + (m_Level - 1) * 15;
}

int PlayerProgression::MaxMana() const
{
    return (m_Profession == 0 ? 80 : 40) + (m_Level - 1) * 5;
}

int PlayerProgression::Mana() const
{
    return m_Mana;
}

bool PlayerProgression::SpendMana(int amount)
{
    if (amount < 0 || m_Mana < amount)
    {
        return false;
    }
    m_Mana -= amount;
    return true;
}

int PlayerProgression::Strength() const
{
    return 5 + (m_Profession == 1 ? (m_Level - 1) * 2 : 0);
}

int PlayerProgression::Intelligence() const
{
    return 5 + (m_Profession == 0 ? (m_Level - 1) * 2 : 0);
}

int PlayerProgression::Attack() const
{
    return 13 + (m_Profession == 0 ? Intelligence() : Strength());
}

bool PlayerProgression::KnightAuraActive() const
{
    return m_Profession == 1 && m_Level >= KnightSkillData::AuraLevel;
}

float PlayerProgression::EffectiveAttack(int equipmentBonus) const
{
    return (Attack() + equipmentBonus) *
           (KnightAuraActive() ? KnightSkillData::AttackMultiplier : 1.f);
}

float PlayerProgression::AttackRangeScale() const
{
    return KnightAuraActive() ? KnightSkillData::RangeMultiplier : 1.f;
}
