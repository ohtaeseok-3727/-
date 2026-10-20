#include "stdafx.h"
#include "GameSession.h"

void GameSession::StrikeArea(WorldPosition center, float radius, int damage)
{
    if (IsDefeated() || m_Chapter->ControlLocked())
        return;
    RewardDefeated(m_Enemies.StrikeArea(LocalPlayer(center), radius, damage));
    if (m_OpenWorld)
    {
        for (std::size_t i = 0; i < m_Regions.size(); ++i)
        {
            if (i == m_Region)
                continue;
            auto& region = m_Regions[i];
            RewardDefeated(region.enemies.StrikeArea(m_Atlas.ToLocal(i, center), radius, damage),
                           region.loot);
        }
    }
}
