#include "stdafx.h"
#include "GameSession.h"
#include <algorithm>
#include <utility>

namespace
{
    const char* StateName(EnemyState state)
    {
        switch (state)
        {
        case EnemyState::Idle:
            return "idle";
        case EnemyState::Chase:
            return "chase";
        case EnemyState::Patrol:
            return "patrol";
        case EnemyState::Return:
            return "return";
        case EnemyState::Windup:
            return "windup";
        case EnemyState::Recovery:
            return "recovery";
        case EnemyState::Heal:
            return "heal";
        case EnemyState::Disabled:
            return "disabled";
        case EnemyState::Dead:
            return "dead";
        }
        return "unknown";
    }

    const char* RoleName(EnemyRole role)
    {
        switch (role)
        {
        case EnemyRole::Melee:
            return "melee";
        case EnemyRole::Ranged:
            return "ranged";
        case EnemyRole::Healer:
            return "healer";
        case EnemyRole::Boss:
            return "boss";
        }
        return "unknown";
    }
}

MonitorSnapshot GameSession::CaptureMonitoring(const LevelMap& map, WorldPosition player) const
{
    MonitorSnapshot snapshot;
    if (!m_Chapter)
    {
        return snapshot;
    }
    snapshot.status = "running";
    snapshot.chapter = m_ActiveChapterId;
    snapshot.playerPosition = player;
    const auto append = [&](const std::string& chapterId,
                            const MapDefinition& definition,
                            const Chapter& chapter,
                            const EnemySystem& enemies,
                            WorldPosition origin,
                            bool active,
                            bool discovered)
    {
        MonitorRegion region;
        region.id = chapterId + ":" + std::to_string(definition.id);
        region.name = definition.title;
        region.kind = std::any_of(definition.areas.begin(),
                                  definition.areas.end(),
                                  [](const MapArea& area)
                                  {
                                      return area.id == MapRegion::Village;
                                  })
                          ? "settlement"
                          : "region";
        region.active = active;
        region.discovered = discovered;
        region.observation =
            chapter.ObserveMap(definition, {player.x - origin.x, player.y - origin.y});
        for (auto& npc : region.observation.npcs)
        {
            npc.id = region.id + ":npc:" + npc.id;
            if (npc.position)
            {
                npc.position->x += origin.x;
                npc.position->y += origin.y;
            }
        }
        for (const auto& enemy : enemies.Enemies())
        {
            MonitorMonster monster;
            monster.id = region.id + ":monster:" + std::to_string(enemy.id);
            monster.role = RoleName(enemy.role);
            monster.state = enemy.health <= 0 ? "dead" : StateName(enemy.ai.state);
            monster.position = {enemy.position.x + origin.x, enemy.position.y + origin.y};
            monster.health = (std::max)(0, enemy.health);
            monster.maxHealth = enemy.maxHealth;
            monster.scripted = enemy.stats.scriptedAI;
            region.monsters.push_back(std::move(monster));
        }
        snapshot.regions.push_back(std::move(region));
    };
    if (m_OpenWorld)
    {
        for (std::size_t i = 0; i < m_Regions.size(); ++i)
        {
            const auto& region = m_Atlas.Regions()[i];
            const auto* chapter = FindChapter(region.chapter);
            if (!chapter)
            {
                continue;
            }
            // The active region's enemies have been moved out of RegionRuntime.
            const auto& enemies = i == m_Region ? m_Enemies : m_Regions[i].enemies;
            append(region.chapter,
                   region.local,
                   *chapter,
                   enemies,
                   region.origin,
                   i == m_Region,
                   region.discovered);
        }
    }
    else
    {
        append(m_ActiveChapterId, map.Layout().Definition(), *m_Chapter, m_Enemies, {}, true, true);
    }
    return snapshot;
}
