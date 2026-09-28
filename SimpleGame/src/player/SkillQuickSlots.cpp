#include "stdafx.h"
#include "SkillQuickSlots.h"

namespace
{
    const char SlotKeys[] = {'a', 's', 'd', 'f', 'q', 'w', 'e', 'r'};
}

void SkillQuickSlots::Reset()
{
    m_Slots.fill("");
}

bool SkillQuickSlots::IsBound(char key) const
{
    const int index = KeyIndex(key);
    return index >= 0 && !m_Slots[index].empty();
}

const std::string& SkillQuickSlots::At(int index) const
{
    static const std::string Empty;
    return index >= 0 && index < static_cast<int>(m_Slots.size()) ? m_Slots[index] : Empty;
}

const std::string& SkillQuickSlots::ForKey(char key) const
{
    return At(KeyIndex(key));
}

void SkillQuickSlots::Assign(int index, const std::string& skillId)
{
    if (index >= 0 && index < static_cast<int>(m_Slots.size()))
    {
        m_Slots[index] = skillId;
    }
}

int SkillQuickSlots::KeyIndex(char key)
{
    for (int i = 0; i < 8; ++i)
    {
        if (SlotKeys[i] == key)
        {
            return i;
        }
    }
    return -1;
}
