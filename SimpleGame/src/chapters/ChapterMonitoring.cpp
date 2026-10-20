#include "stdafx.h"
#include "Chapter1.h"
#include "Chapter2.h"
#include <utility>

MonitorObservation Chapter1::ObserveMap(const MapDefinition& map, WorldPosition player) const
{
    MonitorObservation result;
    if (map.id == Chapter1Maps::Village)
    {
        result.status = m_State.farmRaidResolved ? "stable"
                        : m_State.villageAlert   ? "alert"
                                                 : "watch";
        result.detail = m_State.farmRaidResolved ? "농가 습격 해결 · 마을과 주변 농경지 집계"
                        : m_State.villageAlert   ? "농가 습격 진행 · 주민 경계"
                                                 : "배급 부족 · 농가 상황 확인 전";
    }
    else if (map.id == Chapter1Maps::Forest)
    {
        result.status = m_State.convoyDisabled ? "cleared" : "occupied";
        result.detail = m_State.prisonersReleased ? "포로 구출 완료" : "호송대와 포로 상태 확인";
    }
    else
    {
        result.status = m_State.grumaDefeated ? "cleared" : "occupied";
        result.detail = m_State.grumaDefeated ? "군단장 격파" : "고블린 폐광 기지";
    }
    for (const auto& site : map.landmarks)
    {
        MonitorNpc npc;
        npc.id = std::to_string(site.object);
        npc.position = site.position;
        npc.present = true;
        npc.role = "civilian";
        if (site.object == Chapter1Objects::Ration)
        {
            npc.name = "배급소 주민";
            npc.state = m_State.rationInteractionComplete ? "served" : "waiting";
            npc.detail = m_State.reliefDonationMade ? "구호 식량 전달됨" : "구호 식량 미전달";
        }
        else if (site.object == Chapter1Objects::Farmer)
        {
            npc.name = "농부";
            npc.state = m_State.farmRaidResolved ? "safe" : m_State.villageAlert ? "alert" : "idle";
            npc.detail = "농경지의 주민 NPC";
        }
        else if (site.object == Chapter1Objects::Prisoner)
        {
            npc.name = "호송대 포로";
            npc.state = m_State.prisonersReleased ? "rescued" : "captive";
            npc.present = !m_State.prisonersReleased;
            npc.detail = "주민 집단을 대표하는 1개 NPC 객체 · 실제 주민 인구 수 아님";
            if (!npc.present)
            {
                npc.position.reset();
            }
        }
        else if (site.object == Chapter1Objects::Hero)
        {
            npc.name = "용사 (챕터 1)";
            npc.role = "companion";
            const bool cinematic = m_State.heroArrived && ControlLocked() && m_State.map == map.id;
            npc.present = cinematic || m_State.grumaDefeated || m_State.chapter1Complete;
            npc.state = cinematic ? "scripted" : npc.present ? "idle" : "absent";
            npc.detail = "챕터 1 용사 액터";
            if (cinematic)
            {
                npc.position = WorldPosition{player.x + 42.f, player.y + 8.f};
            }
            else if (!npc.present)
            {
                npc.position.reset();
            }
        }
        else
        {
            continue;
        }
        result.npcs.push_back(std::move(npc));
    }
    return result;
}

MonitorObservation Chapter2::ObserveMap(const MapDefinition& map, WorldPosition) const
{
    MonitorObservation result;
    if (map.id == Chapter2Maps::Graywall)
    {
        const bool supported = Has(Chapter2Event::FrontlineSupported);
        result.status = supported ? "stable" : "watch";
        result.detail =
            supported ? "방어선 지원 완료 · 회색벽과 주변 방어선 집계" : "방어선 지원 전";
    }
    else if (map.id == Chapter2Maps::Canyon)
    {
        result.status = m_State.completedTargets == 3 ? "cleared"
                        : m_State.activeTarget        ? "alert"
                                                      : "watch";
        result.detail =
            "대응 완료 " + std::to_string(m_State.completedTargets) + "/3 · " +
            (m_State.outcome.refugeeMajoritySurvived ? "난민 다수 생존" : "난민 다수 생존 미확정");
    }
    else
    {
        result.status = m_State.outcome.barkanDefeated ? "cleared" : "occupied";
        result.detail = m_State.outcome.barkanDefeated ? "바르칸 격파" : "오크 성채 작전";
    }
    if (map.id < Chapter2Maps::Graywall || map.id > Chapter2Maps::OldFortress)
    {
        return result;
    }
    const auto& combat =
        map.id == m_State.map
            ? m_Combat
            : m_RegionCombat[static_cast<std::size_t>(map.id - Chapter2Maps::Graywall)];
    if (combat.heroPresent || map.id == Chapter2Maps::OldFortress)
    {
        MonitorNpc hero;
        hero.id = "hero";
        hero.name = "용사 (챕터 2)";
        hero.role = "companion";
        hero.present = combat.heroPresent && m_State.phase != Chapter2Phase::Complete;
        hero.state = !hero.present                                  ? "absent"
                     : combat.heroAttack > 0.f                      ? "attacking"
                     : combat.heroWaypoint < combat.heroPath.size() ? "moving"
                                                                    : "idle";
        hero.detail =
            map.id == m_State.map ? "동행 용사 현재 상태" : "비활성 지역에 보관된 동행 상태";
        if (hero.present)
        {
            hero.position = combat.hero;
        }
        result.npcs.push_back(std::move(hero));
    }
    return result;
}
