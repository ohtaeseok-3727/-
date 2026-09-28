#pragma once

#include "SkillCatalog.h"

// Player-facing catalog access and level/passive eligibility. Learning uses the existing level gate only.
class PlayerSkills
{
  public:
    void Reset(int profession);
    const std::vector<SkillData>& Available() const;
    bool CanUse(const SkillData& skill, int level) const;
    const SkillData* Find(const std::string& id) const;

  private:
    SkillCatalog m_Catalog;
    int m_Profession = 0;
};
