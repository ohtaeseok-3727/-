#include "stdafx.h"
#include "PlayerSkills.h"

void PlayerSkills::Reset(int profession)
{
    m_Profession = profession;
}

const std::vector<SkillData>& PlayerSkills::Available() const
{
    return m_Catalog.ForProfession(m_Profession);
}

bool PlayerSkills::CanUse(const SkillData& skill, int level) const
{
    return skill.profession == m_Profession && skill.kind == SkillKind::Active &&
           level >= skill.requiredLevel &&
           (skill.id != "knight_wave" || HasPassive("knight_aura", level));
}

const SkillData* PlayerSkills::Find(const std::string& id) const
{
    return m_Catalog.Find(id);
}

bool PlayerSkills::HasPassive(const std::string& id, int level) const
{
    const auto* skill = Find(id);
    return skill && skill->profession == m_Profession && skill->kind == SkillKind::Passive &&
           level >= skill->requiredLevel;
}
