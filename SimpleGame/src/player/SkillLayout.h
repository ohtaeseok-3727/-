#pragma once

#include <algorithm>

struct SkillLayout
{
    float slotSize = 40.f;
    float slotGap = 5.f;
    float slotsX = 0.f;
    float slotsY = 0.f;
    float panelX = 10.f;
    float panelY = 82.f;
    float panelWidth = 620.f;
    float cardWidth = 162.f;
    bool canShowWindow = false;
    int columns = 3;

    static SkillLayout Create(int width, int height, int skillCount = 6)
    {
        SkillLayout layout;
        layout.columns = skillCount > 6 ? 4 : 3;
        layout.slotGap = 4.f;
        layout.slotSize =
            (std::max)(12.f, (std::min)(42.f, (width - 64.f - layout.slotGap * 7.f) / 8.f));
        layout.slotsX = (width - (layout.slotSize * 8.f + layout.slotGap * 7.f)) * .5f;
        layout.slotsY = (std::max)(0.f, height - 198.f);
        layout.panelWidth = (std::max)(280.f, (std::min)(620.f, width - 20.f));
        layout.panelX = (width - layout.panelWidth) * .5f;
        layout.panelY = (std::max)(8.f, (std::min)(82.f, height - 562.f));
        layout.cardWidth = (layout.panelWidth - 52.f) / layout.columns;
        layout.canShowWindow = width >= 600 && height >= 570;
        return layout;
    }

    float SlotX(int index) const
    {
        return slotsX + index * (slotSize + slotGap);
    }

    float CardX(int index) const
    {
        return panelX + 26.f + (index % columns) * cardWidth;
    }

    float CardY(int index) const
    {
        return panelY + 104.f + (index / columns) * 118.f;
    }

    int SlotAt(float x, float y) const
    {
        for (int i = 0; i < 8; ++i)
        {
            if (x >= SlotX(i) && x <= SlotX(i) + slotSize && y >= slotsY && y <= slotsY + slotSize)
            {
                return i;
            }
        }
        return -1;
    }
};
