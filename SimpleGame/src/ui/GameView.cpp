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
    const auto& progress = session.Progress();
    const auto& inventory = session.Inventory();
    const float panelWidth = (std::min)(350.f, float(width) - 32.f);
    GameUiTheme::Panel(r, 16, 16, panelWidth, 117);
    GameUiTheme::Diamond(r, 51, 57, 24, GameUiTheme::Gold());
    GameUiTheme::Diamond(r, 51, 57, 21, GameUiTheme::Ink());
    GameUiTheme::Center(r, 51, 63, std::to_string(progress.Level()), GameUiTheme::Text(), 1.25f);
    GameUiTheme::Center(r, 51, 97, "레벨", GameUiTheme::Muted());
    r.Text(88, 39, HudTextLayout::Fit(r, region, panelWidth - 90), GameUiTheme::Gold());
    const float meterWidth = (std::max)(8.f, panelWidth - 90.f);
    GameUiTheme::Meter(r,
                       88,
                       49,
                       meterWidth,
                       19,
                       progress.Health(),
                       progress.MaxHealth(),
                       Color(.57f, .16f, .15f));
    GameUiTheme::Center(r,
                        88 + meterWidth * .5f,
                        63,
                        "체력 " + std::to_string(progress.Health()) + " / " +
                            std::to_string(progress.MaxHealth()),
                        GameUiTheme::Text());
    GameUiTheme::Meter(
        r, 88, 75, meterWidth, 15, progress.Mana(), progress.MaxMana(), Color(.13f, .32f, .52f));
    GameUiTheme::Center(r,
                        88 + meterWidth * .5f,
                        87,
                        "마나 " + std::to_string(progress.Mana()) + " / " +
                            std::to_string(progress.MaxMana()),
                        GameUiTheme::Text());
    r.Text(88,
           112,
           "공격력 " +
               std::to_string(static_cast<int>(
                   std::lround(progress.EffectiveAttack(inventory.AttackBonus())))) +
               "   금화 " + std::to_string(inventory.Gold()),
           GameUiTheme::Muted());
    GameUiTheme::Meter(r,
                       24,
                       123,
                       panelWidth - 16,
                       4,
                       progress.Experience(),
                       progress.RequiredExperience(),
                       GameUiTheme::Gold());
    if (width >= 1120)
    {
        GameUiTheme::Panel(r, 16, float(height - 112), 178, 94);
        r.Text(30, float(height - 87), "탐험 수첩", GameUiTheme::Gold());
        r.Text(30, float(height - 62), "[M] 지도   [J] 기록", GameUiTheme::Text());
        r.Text(30, float(height - 37), "[I] 가방   [Esc] 메뉴", GameUiTheme::Muted());
        GameUiTheme::Panel(r, float(width - 194), float(height - 112), 178, 94);
        r.Text(float(width - 180), float(height - 87), "전투와 탐험", GameUiTheme::Gold());
        r.Text(float(width - 180), float(height - 62), "[Ctrl] 연계 공격", GameUiTheme::Text());
        r.Text(float(width - 180),
               float(height - 37),
               "[Space] 확인 · 상호작용",
               GameUiTheme::Muted());
    }
    const float noticeWidth = (std::min)(680.f, float(width) - 48.f);
    if (!session.Notice().empty())
    {
        const float noticeX = (width - noticeWidth) * .5f;
        GameUiTheme::Panel(r, noticeX, float(height - 183), noticeWidth, 34);
        r.Text(noticeX + 12,
               float(height - 160),
               HudTextLayout::Fit(r, session.Notice(), noticeWidth - 24),
               GameUiTheme::Text());
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
