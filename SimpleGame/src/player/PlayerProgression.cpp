#include "stdafx.h"
#include "PlayerProgression.h"
#include "KnightSkillData.h"
#include <algorithm>
#include <cmath>

void PlayerProgression::Reset(int profession)
{
    m_Profession = profession;
    m_Level = 1;
    m_Experience = 0;
    m_Health = MaxHealth();
    m_Mana = MaxMana();
    m_ManaRecoveryRemainder = 0.0;
}

void PlayerProgression::DebugSetLevel(int level)
{
    m_Level = (std::max)(1, level);
    m_Experience = 0;
    m_Health = MaxHealth();
    m_Mana = MaxMana();
    m_ManaRecoveryRemainder = 0.0;
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
    m_ManaRecoveryRemainder = 0.0;
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

void PlayerProgression::RecoverMana(double amount)
{
    if (!std::isfinite(amount) || amount <= 0.0 || m_Health <= 0)
    {
        return;
    }
    const int missing = MaxMana() - m_Mana;
    if (missing <= 0)
    {
        m_ManaRecoveryRemainder = 0.0;
        return;
    }
    // Retain sub-point recovery, but never bank regeneration beyond full mana.
    const double recovered =
        (std::min)(static_cast<double>(missing), amount + m_ManaRecoveryRemainder);
    const int whole = static_cast<int>(recovered);
    m_Mana += whole;
    m_ManaRecoveryRemainder = recovered - whole;
}

void PlayerProgression::UpdateManaRecovery(float seconds)
{
    if (!std::isfinite(seconds) || seconds <= 0.f)
    {
        return;
    }
    RecoverMana(MaxMana() * 0.01 * seconds);
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
