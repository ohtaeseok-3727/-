#include "stdafx.h"
#include "GameView.h"
#include "HudTextLayout.h"
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
    r.Rect(0, 0, float(width), 116, Color(.025f, .04f, .045f, .96f));
    r.Text(20,
           23,
           std::string(session.StoryChapter().Title()) + " / " + region,
           Color(.87f, .79f, .57f));
    std::ostringstream stats;
    stats << "레벨 " << progress.Level() << "   체력 " << progress.Health() << "/"
          << progress.MaxHealth() << "   공격력 "
          << static_cast<int>(std::lround(progress.EffectiveAttack(inventory.AttackBonus())))
          << "   마나 " << progress.Mana() << "/" << progress.MaxMana() << "   보유금 "
          << inventory.Gold() << "   장비 " << inventory.Items().size();
    r.Text(20, 44, stats.str(), Color(.83f, .85f, .78f));
    r.Rect(20, 54, 200, 8, Color(.18f, .09f, .08f));
    r.Rect(20, 54, 200.f * progress.Health() / progress.MaxHealth(), 8, Color(.72f, .25f, .19f));
    r.Rect(240, 54, 200, 8, Color(.08f, .15f, .18f));
    r.Rect(240,
           54,
           200.f * progress.Experience() / progress.RequiredExperience(),
           8,
           Color(.29f, .69f, .80f));
    r.Text(455,
           62,
           "경험치 " + std::to_string(progress.Experience()) + " / " +
               std::to_string(progress.RequiredExperience()),
           Color(.65f, .83f, .86f));
    r.Text(20,
           85,
           HudTextLayout::Fit(r, session.Objective(), float(width) - 40.f),
           Color(.84f, .80f, .64f));
    r.Text(
        20,
        105,
        HudTextLayout::Fit(
            r,
            "방향키/WASD 이동 · Shift 달리기 · Ctrl 공격 · Space 확인 · M 지도 · J 기록 · I 가방 · K 스킬 · Esc 메뉴 · B 야영지",
            float(width) - 40.f),
        Color(.57f, .65f, .61f));
    r.Rect(0, float(height - 42), float(width), 42, Color(.025f, .04f, .045f, .95f));
    r.Text(20, float(height - 17), session.Notice(), Color(.95f, .84f, .57f));
    m_InventoryView.Draw(r, inventory, width, height);
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
