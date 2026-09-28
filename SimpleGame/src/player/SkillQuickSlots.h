#pragma once

#include <array>
#include <string>

class SkillQuickSlots
{
  public:
    void Reset();
    bool IsBound(char key) const;
    const std::string& At(int index) const;
    const std::string& ForKey(char key) const;
    void Assign(int index, const std::string& skillId);
    static int KeyIndex(char key);

  private:
    std::array<std::string, 8> m_Slots{};
};
