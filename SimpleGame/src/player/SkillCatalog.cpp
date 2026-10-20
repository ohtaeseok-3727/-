#include "stdafx.h"
#include "SkillCatalog.h"
#include "KnightSkillData.h"

SkillCatalog::SkillCatalog()
    : m_Skills{
          {"demo_mage_bolt", "임시 액티브 1", 0, 1, SkillKind::Active, 10, 2.f, 0.f, 0.f, 0},
          {"demo_mage_focus", "임시 패시브", 0, 3, SkillKind::Passive, 0, 0.f, 0.f, 0.f, 1},
          {"demo_mage_wave", "임시 액티브 2", 0, 1, SkillKind::Active, 18, 4.f, 0.f, 0.f, 2},
          {"demo_mage_locked", "임시 잠금 액티브", 0, 5, SkillKind::Active, 24, 5.f, 0.f, 0.f, 3},
          {"mage_wind_blades", "바람 칼날", 0, 3, SkillKind::Active, 3, 2.f, 100.f, 0.7f, 4, 3},
          {"mage_mana_swords", "마력검 사출", 0, 5, SkillKind::Active, 7, 5.f, 100.f, 1.f, 5, 5},
          {"mage_sword_song",
           "마법과 검의 노래",
           0,
           20,
           SkillKind::Active,
           50,
           60.f,
           10.f,
           .2f,
           6,
           40,
           400.f,
           6.f},
          {"knight_thrust", "연속 찌르기", 1, 3, SkillKind::Active, 4, 5.f, 70.f, .5f, 0, 6},
          {"knight_aura",
           "검기 상인",
           1,
           KnightSkillData::AuraLevel,
           SkillKind::Passive,
           0,
           0.f,
           0.f,
           0.f,
           1},
          {"knight_wave", "검기 발산", 1, 5, SkillKind::Active, 10, 10.f, 150.f, 1.f, 2, 1},
          {"knight_cheondan", "천단참", 1, 20, SkillKind::Active, 30, 60.f, 500.f, 5.f, 3, 1}}
{
    for (const SkillData& skill : m_Skills)
    {
        (skill.profession == 0 ? m_MageSkills : m_KnightSkills).push_back(skill);
    }
}

const std::vector<SkillData>& SkillCatalog::ForProfession(int profession) const
{
    return profession == 0 ? m_MageSkills : m_KnightSkills;
}

const SkillData* SkillCatalog::Find(const std::string& id) const
{
    for (const SkillData& skill : m_Skills)
    {
        if (skill.id == id)
        {
            return &skill;
        }
    }
    return nullptr;
}
