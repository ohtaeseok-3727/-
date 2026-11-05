#include "stdafx.h"
#include "PrototypeWorld.h"
#include <cmath>

void PrototypeWorld::ToggleTrade()
{
    if (m_Trade.IsOpen())
    {
        m_Trade.Close();
        return;
    }
    if (!m_Started || m_Paused || m_Level.IsDefeated() || m_SettingsMenu.IsOpen() ||
        m_Level.CurrentChapter().ControlLocked())
    {
        return;
    }
    bool nearShop = false;
    for (const auto& site : m_Map.Layout().Landmarks())
    {
        if ((site.kind == LandmarkKind::Shop || site.kind == LandmarkKind::Trader) &&
            std::hypot(site.position.x - m_X, site.position.y - m_Y) <= 120.f)
        {
            nearShop = true;
            break;
        }
    }
    if (!nearShop || m_Level.InCombat({m_X, m_Y}) || m_Attack.IsAttacking() ||
        m_MageProjectiles.Casting() || m_MageUltimate.Active() || m_KnightSkills.Casting())
    {
        m_Level.ShowSkillNotice("마을 또는 아르겐 보급 상인 근처에서 전투를 마친 뒤 거래하세요.");
        return;
    }
    CloseMenus();
    m_Trade.Toggle();
    m_Attack.Reset();
    m_Moving = false;
}

void PrototypeWorld::UsePotion(bool mana)
{
    if (!m_Started || m_Paused || MenuOpen() || m_Level.IsDefeated() ||
        m_Level.CurrentChapter().ControlLocked())
    {
        return;
    }
    auto& progress = m_Level.Progress();
    if ((mana ? progress.Mana() >= progress.MaxMana() : progress.Health() >= progress.MaxHealth()))
    {
        m_Level.ShowSkillNotice(mana ? "마나가 가득 차 물약을 사용하지 않았습니다."
                                     : "체력이 가득 차 물약을 사용하지 않았습니다.");
        return;
    }
    if (!m_Level.Inventory().UsePotion(mana))
    {
        m_Level.ShowSkillNotice(mana ? "마나 물약이 없습니다. 보급 상인에게 구매하세요."
                                     : "체력 물약이 없습니다. 보급 상인에게 구매하세요.");
        return;
    }
    if (mana)
    {
        progress.RecoverMana(TradeSystem::ManaRecovery);
    }
    else
    {
        progress.Heal(TradeSystem::HealthRecovery);
    }
    m_Level.ShowSkillNotice(mana ? "마나 물약 사용 · MP +30" : "체력 물약 사용 · HP +50");
}
