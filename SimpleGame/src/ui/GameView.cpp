#include "stdafx.h"
#include "GameView.h"
#include "HudTextLayout.h"
#include "GameUiTheme.h"
#include <algorithm>
#include <cmath>
#include <sstream>

void GameView::DrawDrop(Renderer& r, const LootDrop& drop, Point p) const
{
    const float bob = std::sin(drop.age * 6.f) * 2.f;
    r.Ellipse(p.x, p.y, 7, 3, Color(.02f, .03f, .02f, .4f));
    if (drop.kind == LootKind::Gold)
    {
        r.Ellipse(p.x, p.y - 8 + bob, 5, 6, Color(1.f, .77f, .20f));
    }
    else if (drop.kind == LootKind::Health)
    {
        r.Rect(p.x - 6, p.y - 12 + bob, 12, 10, Color(.85f, .18f, .20f));
        r.Rect(p.x - 1, p.y - 11 + bob, 2, 8, Color(1.f, .88f, .84f));
        r.Rect(p.x - 4, p.y - 8 + bob, 8, 2, Color(1.f, .88f, .84f));
    }
    else
    {
        r.Quad({p.x, p.y - 17 + bob},
               {p.x + 7, p.y - 9 + bob},
               {p.x, p.y - 1 + bob},
               {p.x - 7, p.y - 9 + bob},
               Color(.30f, .70f, 1.f));
    }
}

void GameView::DrawHud(
    Renderer& r, const GameSession& session, const char* region, int width, int height) const
{
    const auto& p = session.Progress();
    const auto& inventory = session.Inventory();
    // The bottom bar reserves its right-hand area for mouse navigation.
    r.Rect(0, height - 114.f, float(width), 114.f, GameUiTheme::Ink());
    r.Rect(0, height - 114.f, float(width), 3, GameUiTheme::Border());
    r.Rect(0, height - 111.f, float(width), 1, GameUiTheme::Border());
    const float panelWidth = (std::max)(120.f, width * .43f - 22.f);
    const float top = height - 106.f;
    GameUiTheme::Panel(r, 10, top, panelWidth, 83);
    r.Text(20,
           top + 19,
           "Lv." + std::to_string(p.Level()) + "  " +
               HudTextLayout::Fit(r, region, panelWidth - 100),
           GameUiTheme::Text());
    const float meterWidth = (std::max)(60.f, panelWidth - 70.f);
    r.Text(20, top + 41, "HP", Color(.72f, .43f, .45f));
    r.Text(20, top + 65, "MP", Color(.42f, .58f, .74f));
    GameUiTheme::Meter(
        r, 58, top + 26, meterWidth, 19, p.Health(), p.MaxHealth(), Color(.42f, .13f, .17f));
    GameUiTheme::Meter(
        r, 58, top + 50, meterWidth, 19, p.Mana(), p.MaxMana(), Color(.12f, .26f, .43f));
    GameUiTheme::Center(r,
                        58 + meterWidth * .5f,
                        top + 40,
                        std::to_string(p.Health()) + " / " + std::to_string(p.MaxHealth()),
                        GameUiTheme::Text());
    GameUiTheme::Center(r,
                        58 + meterWidth * .5f,
                        top + 64,
                        std::to_string(p.Mana()) + " / " + std::to_string(p.MaxMana()),
                        GameUiTheme::Text());
    GameUiTheme::Meter(r,
                       0,
                       height - 14.f,
                       float(width),
                       14,
                       p.Experience(),
                       p.RequiredExperience(),
                       Color(.40f, .30f, .115f));
    GameUiTheme::Center(r,
                        width * .5f,
                        height - 3.f,
                        "EXP " + std::to_string(p.Experience()) + " / " +
                            std::to_string(p.RequiredExperience()),
                        GameUiTheme::Text());
    if (width >= 760)
    {
        GameUiTheme::Panel(r, 16, 16, 240, 54);
        r.Text(28, 38, HudTextLayout::Fit(r, region, 216), GameUiTheme::Text());
        r.Text(28,
               58,
               "공격 " +
                   std::to_string(
                       static_cast<int>(std::lround(p.EffectiveAttack(inventory.AttackBonus())))) +
                   "  금화 " + std::to_string(inventory.Gold()),
               GameUiTheme::Muted());
    }
    if (!session.Notice().empty())
    {
        const float w = (std::min)(680.f, width - 32.f), x = (width - w) * .5f, y = height - 266.f;
        GameUiTheme::Panel(r, x, y, w, 34);
        r.Text(
            x + 12, y + 23, HudTextLayout::Fit(r, session.Notice(), w - 24), GameUiTheme::Text());
    }
}

void GameView::ToggleInventory()
{
    m_InventoryView.Toggle();
}

void GameView::CloseInventory()
{
    m_InventoryView.Close();
}

void GameView::ChangePage(int direction, int itemCount)
{
    m_InventoryView.ChangePage(direction, itemCount);
}

void GameView::DrawInventory(Renderer& r, const GameSession& session, int width, int height) const
{
    m_InventoryView.Draw(r, session.Inventory(), width, height);
}

void GameView::InventoryMouseButton(
    bool down, int x, int y, int width, int height, PlayerInventory& inventory)
{
    m_InventoryView.MouseButton(down, x, y, width, height, inventory);
}

void GameView::CancelInventoryPress()
{
    m_InventoryView.CancelPress();
}
