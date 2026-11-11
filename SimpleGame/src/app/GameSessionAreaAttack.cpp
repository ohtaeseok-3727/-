#include "stdafx.h"
#include "GameSession.h"

void GameSession::StrikeArea(WorldPosition center, float radius, int damage, const DamageCast& cast)
{
    if (IsDefeated() || m_Chapter->ControlLocked())
        return;
    RewardDefeated(m_Enemies.StrikeArea(LocalPlayer(center), radius, damage, cast));
    if (m_OpenWorld)
    {
        for (std::size_t i = 0; i < m_Regions.size(); ++i)
        {
            if (i == m_Region)
                continue;
            auto& region = m_Regions[i];
            RewardDefeated(
                region.enemies.StrikeArea(m_Atlas.ToLocal(i, center), radius, damage, cast),
                region.loot);
        }
    }
}
