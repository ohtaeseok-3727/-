#pragma once

#include <string>
#include <vector>

enum class SkillKind
{
    Active,
    Passive
};

struct SkillData
{
    std::string id;
    std::string name;
    int profession = 0;
    int requiredLevel = 1;
    SkillKind kind = SkillKind::Active;
    int manaCost = 0;
    float cooldown = 0.f;
    float attackRatio = 0.f;
    float statRatio = 0.f;
    int iconVariant = 0;
    int projectileCount = 1;
};

// Demo-only catalog. Names and numeric values are intentionally not game balance decisions.
class SkillCatalog
{
  public:
    SkillCatalog();
    const std::vector<SkillData>& ForProfession(int profession) const;
    const SkillData* Find(const std::string& id) const;

  private:
    std::vector<SkillData> m_Skills;
    std::vector<SkillData> m_MageSkills;
    std::vector<SkillData> m_KnightSkills;
};
