#include "stdafx.h"
#include "GameSession.h"
#include "Chapter3.h"
#include <algorithm>

bool GameSession::TravelThroughPortal(const ChapterTravel& travel, WorldPosition& destination)
{
    if (!m_OpenWorld || !travel.map || !travel.destination ||
        (!travel.chapter.empty() && travel.chapter != m_ActiveChapterId))
    {
        return false;
    }
    const auto& source = m_Atlas.Regions()[m_Region];
    const auto& portals = source.local.portals;
    const auto portal = std::find_if(portals.begin(),
                                     portals.end(),
                                     [&travel](const MapPortal& item)
                                     {
                                         return item.destination == *travel.map &&
                                                item.arrival.x == travel.destination->x &&
                                                item.arrival.y == travel.destination->y;
                                     });
    const auto target = m_Atlas.Find(m_ActiveChapterId, *travel.map);
    if (portal == portals.end() || !target ||
        m_Regions[*target].map.Blocked(travel.destination->x, travel.destination->y))
    {
        ShowNotice("기록관 포탈의 목적지를 사용할 수 없습니다.");
        return false;
    }
    ActivateRegion(*target);
    SynchronizeChapterHistory();
    m_Chapter->PlayerArrived(*travel.destination, m_Regions[*target].map);
    destination = m_Atlas.ToWorld(*target, *travel.destination);
    m_Push = {};
    m_LandmarkInteractionTime = 0.f;
    m_LandmarkInteractionText.clear();
    m_Invulnerable = 1.f;
    ShowNotice("기록관 포탈 이동 · " + m_Atlas.Regions()[*target].local.title);
    return true;
}

std::optional<WorldMarker> GameSession::PortalGoal(std::size_t target) const
{
    if (target == m_Region)
    {
        return std::nullopt;
    }
    const auto& current = m_Atlas.Regions()[m_Region];
    const auto& goal = m_Atlas.Regions()[target];
    // Leave a room first. Other interiors are reached through their exterior entrance.
    std::size_t source = m_Region;
    MapId destination = goal.local.id;
    if (current.local.portalInterior)
    {
        const auto exterior = std::find_if(current.local.portals.begin(),
                                           current.local.portals.end(),
                                           [](const MapPortal& portal)
                                           {
                                               return portal.destination == Chapter3Maps::Argen;
                                           });
        if (exterior == current.local.portals.end())
        {
            return std::nullopt;
        }
        destination = exterior->destination;
    }
    else if (goal.local.portalInterior)
    {
        const auto exterior = m_Atlas.Find(goal.chapter, Chapter3Maps::Argen);
        if (!exterior)
        {
            return std::nullopt;
        }
        source = *exterior;
    }
    else
    {
        return std::nullopt;
    }
    const auto& region = m_Atlas.Regions()[source];
    for (const auto& portal : region.local.portals)
    {
        if (portal.destination != destination)
        {
            continue;
        }
        for (const auto& site : region.local.landmarks)
        {
            if (site.object == portal.object)
            {
                return WorldMarker{m_Atlas.ToWorld(source, site.position),
                                   site.name,
                                   "[Space] 기록관 포탈 이용 후 목표를 계속 진행하세요."};
            }
        }
    }
    return std::nullopt;
}
