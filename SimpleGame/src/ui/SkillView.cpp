#include "stdafx.h"
#include "SkillView.h"
#include "SkillSystem.h"
#include "Renderer.h"
#include "SkillLayout.h"
#include "HudTextLayout.h"
#include "GameUiTheme.h"
#include <cctype>
#include <cmath>
#include <string>

namespace
{
    const char SlotKeys[] = {'a', 's', 'd', 'f', 'q', 'w', 'e', 'r'};

    void DrawIcon(Renderer& renderer, float x, float y, int variant, bool dim, float radius = 15.f)
    {
        const Color colors[] = {
            Color(.16f, .31f, .44f), Color(.32f, .23f, .40f), Color(.45f, .25f, .17f)};
        Color color = colors[variant % 3];
        if (dim)
            color = Color(color.r * .3f, color.g * .3f, color.b * .3f);
        renderer.Ellipse(x, y, radius, radius, color);
        const Color mark = dim ? Color(.24f, .26f, .27f) : Color(.58f, .51f, .36f);
        if (variant % 3 == 0)
        {
            GameUiTheme::Diamond(renderer, x, y, radius * .72f, mark);
            GameUiTheme::Diamond(renderer, x, y, radius * .38f, color);
            renderer.Rect(x - 1, y - radius, 2, radius * 2, mark);
        }
        else if (variant % 3 == 1)
        {
            renderer.Quad({x - radius * .55f, y + radius * .65f},
                          {x - radius * .22f, y + radius * .75f},
                          {x + radius * .62f, y - radius * .70f},
                          {x + radius * .12f, y - radius * .50f},
                          mark);
            renderer.Rect(x - radius * .68f, y + radius * .30f, radius, 3, mark);
        }
        else
        {
            renderer.Ellipse(x, y, radius * .64f, radius * .64f, mark);
            renderer.Ellipse(x, y, radius * .38f, radius * .38f, color);
            GameUiTheme::Diamond(renderer, x, y, radius * .19f, mark);
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
    const float barWidth = layout.slotSize * 8 + layout.slotGap * 7;
    GameUiTheme::Panel(
        renderer, layout.slotsX - 8, layout.slotsY - 5, barWidth + 16, layout.slotSize + 10);
    for (int i = 0; i < 8; ++i)
    {
        const float x = layout.SlotX(i);
        GameUiTheme::Slot(renderer, x, layout.slotsY, layout.slotSize, i == hovered);
        if (layout.slotSize >= 12.f)
        {
            const std::string label =
                layout.slotSize >= 32.f
                    ? std::string("[") + static_cast<char>(std::toupper(SlotKeys[i])) + "]"
                    : std::string(1, static_cast<char>(std::toupper(SlotKeys[i])));
            renderer.Text(x + 2.f, layout.slotsY + 15.f, label, GameUiTheme::Text());
        }
        const SkillData* data = skills.PlayerSkillsState().Find(skills.QuickSlots().At(i));
        if (data)
        {
            const auto cooldown = skills.Cooldowns().find(data->id);
            const bool cooling = cooldown != skills.Cooldowns().end();
            DrawIcon(renderer,
                     x + layout.slotSize * .5f,
                     layout.slotsY + layout.slotSize * .68f,
                     data->iconVariant,
                     cooling,
                     (std::min)(15.f, layout.slotSize * .24f));
            if (cooling && layout.slotSize >= 32.f)
            {
                const std::string seconds =
                    std::to_string((std::max)(1, static_cast<int>(std::ceil(cooldown->second))));
                const float textWidth = renderer.TextWidth(seconds);
                renderer.Rect(x + (layout.slotSize - textWidth) * .5f - 3.f,
                              layout.slotsY + layout.slotSize * .68f - 12.f,
                              textWidth + 6.f,
                              19.f,
                              Color(.02f, .03f, .04f, .94f));
                renderer.Text(x + (layout.slotSize - textWidth) * .5f,
                              layout.slotsY + layout.slotSize * .68f + 3.f,
                              seconds,
                              Color(.98f, .88f, .60f));
            }
        }
        else if (layout.slotSize >= 32.f)
        {
            renderer.Text(x + layout.slotSize * .4f,
                          layout.slotsY + layout.slotSize * .8f,
                          "-",
                          GameUiTheme::Muted());
        }
    }
    if (!skills.IsWindowOpen())
    {
        const bool movementBound = skills.IsSlotBound('w') || skills.IsSlotBound('a') ||
                                   skills.IsSlotBound('s') || skills.IsSlotBound('d');
        const std::string hint = movementBound ? "방향키 이동 · 슬롯 클릭/단축키 사용"
                                               : "슬롯 클릭/단축키 사용 · K 스킬 등록";
        GameUiTheme::Panel(renderer, layout.slotsX - 8, layout.slotsY - 30, barWidth + 16, 24);
        renderer.Text(layout.slotsX,
                      layout.slotsY - 10.f,
                      HudTextLayout::Fit(renderer, hint, barWidth - 8.f),
                      GameUiTheme::Text());
        return;
    }
    if (!layout.canShowWindow)
    {
        const std::string message = "스킬 창: 창을 더 키우세요 [K]";
        if (layout.slotsY > 45.f && renderer.TextWidth(message) < width - 20.f)
        {
            renderer.Text(10.f, layout.slotsY - 20.f, message, GameUiTheme::Gold());
        }
        return;
    }
    const std::string profession =
        !list.empty() && list.front().profession == 0 ? "마법사" : "기사";
    GameUiTheme::Window(
        renderer, layout.panelX, layout.panelY, layout.panelWidth, 350.f, profession + " 스킬 창");
    renderer.Text(layout.panelX + 18.f,
                  layout.panelY + 55.f,
                  (profession == "기사"
                       ? "액티브를 슬롯으로 드래그 · 검기 상인은 Lv.5부터 자동 적용"
                       : "액티브를 슬롯으로 드래그 · 궁극기는 Lv.20부터 사용"),
                  GameUiTheme::Muted());
    renderer.Text(layout.panelX + 18.f,
                  layout.panelY + 77.f,
                  "WASD에 스킬을 등록하면 방향키로 이동하세요. [K] 닫기",
                  GameUiTheme::Text());
    for (int i = 0; i < static_cast<int>(list.size()); ++i)
    {
        const SkillData& skill = list[i];
        const bool available = level >= skill.requiredLevel;
        const float x = layout.CardX(i), y = layout.CardY(i);
        renderer.Rect(x,
                      y,
                      layout.cardWidth - 8.f,
                      94.f,
                      available ? Color(.91f, .96f, .98f) : Color(.89f, .89f, .86f));
        DrawIcon(renderer, x + 22.f, y + 28.f, skill.iconVariant, !available);
        renderer.Text(
            x + 46.f, y + 24.f, "Lv." + std::to_string(skill.requiredLevel), GameUiTheme::Muted());
        renderer.Text(x + 46.f,
                      y + 45.f,
                      (skill.id == "knight_cheondan" || skill.id == "mage_sword_song") ? "궁극기"
                      : skill.kind == SkillKind::Active                                ? "액티브"
                      : available && skill.id == "knight_aura"                         ? "적용 중"
                                                                                       : "패시브",
                      GameUiTheme::Muted());
        if (skill.kind == SkillKind::Active)
        {
            renderer.Text(x + 6.f,
                          y + 92.f,
                          "MP " + std::to_string(skill.manaCost) + " / " +
                              std::to_string(static_cast<int>(skill.cooldown)) + "초",
                          GameUiTheme::Muted());
        }
        const Color nameColor = available ? GameUiTheme::Text() : GameUiTheme::Muted();
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
