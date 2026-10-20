#include "stdafx.h"
#include "SkillView.h"
#include "SkillSystem.h"
#include "Renderer.h"
#include "SkillLayout.h"
#include "HudTextLayout.h"
#include <cctype>
#include <string>

namespace
{
    const char SlotKeys[] = {'a', 's', 'd', 'f', 'q', 'w', 'e', 'r'};

    void DrawIcon(Renderer& renderer, float x, float y, int variant, bool dim, float radius = 15.f)
    {
        const Color colors[] = {
            Color(.31f, .68f, .92f), Color(.63f, .46f, .84f), Color(.87f, .47f, .27f)};
        Color color = colors[variant % 3];
        if (dim)
            color = Color(color.r * .3f, color.g * .3f, color.b * .3f);
        renderer.Ellipse(x, y, radius, radius, color);
        if (radius >= 10.f)
        {
            renderer.Text(x - 4.f,
                          y + 5.f,
                          std::to_string(variant + 1),
                          dim ? Color(.4f, .4f, .4f) : Color(.95f, .92f, .70f));
        }
    }
}

void SkillView::Draw(
    Renderer& renderer, const SkillSystem& skills, int width, int height, int level) const
{
    const auto& list = skills.PlayerSkillsState().Available();
    const SkillLayout layout = SkillLayout::Create(width, height, static_cast<int>(list.size()));
    const SkillDragController& drag = skills.Drag();
    const int hovered =
        drag.SkillIndex() >= 0 ? layout.SlotAt(float(drag.MouseX()), float(drag.MouseY())) : -1;
    for (int i = 0; i < 8; ++i)
    {
        const float x = layout.SlotX(i);
        renderer.Rect(x,
                      layout.slotsY,
                      layout.slotSize,
                      layout.slotSize,
                      i == hovered ? Color(.25f, .34f, .29f) : Color(.04f, .07f, .085f, .96f));
        renderer.Rect(x, layout.slotsY, layout.slotSize, 2.f, Color(.73f, .62f, .34f));
        if (layout.slotSize >= 12.f)
        {
            const std::string label =
                layout.slotSize >= 32.f
                    ? std::string("[") + static_cast<char>(std::toupper(SlotKeys[i])) + "]"
                    : std::string(1, static_cast<char>(std::toupper(SlotKeys[i])));
            renderer.Text(x + 2.f, layout.slotsY + 15.f, label, Color(.86f, .79f, .56f));
        }
        const SkillData* data = skills.PlayerSkillsState().Find(skills.QuickSlots().At(i));
        if (data)
        {
            const bool cooling = skills.Cooldowns().count(data->id) != 0;
            DrawIcon(renderer,
                     x + layout.slotSize * .5f,
                     layout.slotsY + layout.slotSize * .68f,
                     data->iconVariant,
                     cooling,
                     (std::min)(15.f, layout.slotSize * .24f));
        }
        else if (layout.slotSize >= 32.f)
        {
            renderer.Text(x + layout.slotSize * .4f,
                          layout.slotsY + layout.slotSize * .8f,
                          "-",
                          Color(.38f, .46f, .48f));
        }
    }
    if (!skills.IsWindowOpen())
        return;
    if (!layout.canShowWindow)
    {
        const std::string message = "스킬 창: 창을 더 키우세요 [K]";
        if (layout.slotsY > 45.f && renderer.TextWidth(message) < width - 20.f)
        {
            renderer.Text(10.f, layout.slotsY - 20.f, message, Color(1.f, .8f, .5f));
        }
        return;
    }
    renderer.Rect(
        layout.panelX, layout.panelY, layout.panelWidth, 330.f, Color(.025f, .045f, .06f, .97f));
    const std::string profession =
        !list.empty() && list.front().profession == 0 ? "마법사" : "기사";
    renderer.Text(layout.panelX + 18.f,
                  layout.panelY + 31.f,
                  profession + " 스킬 창 [K]",
                  Color(.92f, .83f, .62f));
    renderer.Text(layout.panelX + 18.f,
                  layout.panelY + 55.f,
                  (profession == "기사"
                       ? "액티브를 슬롯으로 드래그 · 검기 상인은 Lv.5부터 자동 적용"
                       : "액티브를 슬롯으로 드래그 · 궁극기는 Lv.20부터 사용"),
                  Color(.58f, .72f, .75f));
    for (int i = 0; i < static_cast<int>(list.size()); ++i)
    {
        const SkillData& skill = list[i];
        const bool available = level >= skill.requiredLevel;
        const float x = layout.CardX(i), y = layout.CardY(i);
        renderer.Rect(x,
                      y,
                      layout.cardWidth - 8.f,
                      94.f,
                      available ? Color(.08f, .13f, .15f) : Color(.045f, .055f, .06f));
        DrawIcon(renderer, x + 22.f, y + 28.f, skill.iconVariant, !available);
        renderer.Text(x + 46.f,
                      y + 24.f,
                      "Lv." + std::to_string(skill.requiredLevel),
                      Color(.70f, .78f, .72f));
        renderer.Text(x + 46.f,
                      y + 45.f,
                      (skill.id == "knight_cheondan" || skill.id == "mage_sword_song") ? "궁극기"
                      : skill.kind == SkillKind::Active                                ? "액티브"
                      : available && skill.id == "knight_aura"                         ? "적용 중"
                                                                                       : "패시브",
                      Color(.62f, .71f, .77f));
        if (skill.kind == SkillKind::Active)
        {
            renderer.Text(x + 6.f,
                          y + 92.f,
                          "MP " + std::to_string(skill.manaCost) + " / " +
                              std::to_string(static_cast<int>(skill.cooldown)) + "초",
                          Color(.66f, .74f, .76f));
        }
        const Color nameColor = available ? Color(.84f, .89f, .88f) : Color(.39f, .43f, .43f);
        if (skill.id == "mage_sword_song")
        {
            renderer.Text(x + 6.f, y + 65.f, "마법과 검의", nameColor);
            renderer.Text(x + 6.f, y + 79.f, "노래", nameColor);
        }
        else
        {
            renderer.Text(x + 6.f,
                          y + 76.f,
                          HudTextLayout::Fit(renderer, skill.name, layout.cardWidth - 20.f),
                          nameColor);
        }
    }
    if (drag.SkillIndex() >= 0)
    {
        const SkillData& skill = list[drag.SkillIndex()];
        DrawIcon(renderer, float(drag.MouseX()), float(drag.MouseY()), skill.iconVariant, false);
    }
}
