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

    static SkillLayout Create(int width, int height)
    {
        SkillLayout layout;
        layout.slotGap = (std::min)(7.f, width * .012f);
        layout.slotSize = (std::min)(58.f, (width * .96f - layout.slotGap * 7.f) / 8.f);
        layout.slotsX = (width - (layout.slotSize * 8.f + layout.slotGap * 7.f)) * .5f;
        layout.slotsY = (std::max)(0.f, height - layout.slotSize - 54.f);
        layout.panelWidth = (std::max)(280.f, (std::min)(620.f, width - 20.f));
        layout.panelX = (width - layout.panelWidth) * .5f;
        layout.panelY = 82.f;
        layout.cardWidth = (layout.panelWidth - 52.f) / 3.f;
        layout.canShowWindow = width >= 600 && height >= 570;
        return layout;
    }

    float SlotX(int index) const
    {
        return slotsX + index * (slotSize + slotGap);
    }

    float CardX(int index) const
    {
        return panelX + 26.f + (index % 3) * cardWidth;
    }

    float CardY(int index) const
    {
        return panelY + 104.f + (index / 3) * 118.f;
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
