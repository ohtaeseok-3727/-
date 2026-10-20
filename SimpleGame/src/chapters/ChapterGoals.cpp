#include "stdafx.h"
#include "Chapter1.h"
#include "Chapter2.h"

void Chapter1::VisitMap(MapId map)
{
    // Encounter flags cover the whole chapter; walking back does not respawn cleared groups.
    m_State.map = map;
    ClearTravel();
}

std::optional<ChapterGoal> Chapter1::Goal() const
{
    using namespace Chapter1Objects;
    const auto goal = [](MapId map, ObjectId object)
    {
        return ChapterGoal{map, object, std::nullopt};
    };
    if (m_State.chapter1Complete || ControlLocked())
    {
        return std::nullopt;
    }
    if (!m_State.rationInteractionComplete)
    {
        return goal(Chapter1Maps::Village, Ration);
    }
    if (!m_State.farmRaidResolved)
    {
        return ChapterGoal{Chapter1Maps::Village,
                           FoodEvidence,
                           std::nullopt,
                           "농가 습격 저지",
                           "[Ctrl] 공격 · 주황색 범위 회피",
                           {0.f, 110.f}};
    }
    const ObjectId evidence[] = {FoodEvidence, RepairEvidence, TransportEvidence};
    for (int i = 0; i < 3; ++i)
    {
        if (!m_State.investigationEvidence.test(i))
        {
            return goal(Chapter1Maps::Village, evidence[i]);
        }
    }
    const ObjectId clues[] = {LightTracks, HeavyTracks, Grain, Bark, Rope};
    for (int i = 1; i < 5; ++i)
    {
        if (!m_State.trackingClues.test(i))
        {
            return goal(Chapter1Maps::Forest, clues[i]);
        }
    }
    if (!m_State.convoySpotted)
    {
        return goal(Chapter1Maps::Forest, Ridge);
    }
    if (!m_State.convoyDisabled)
    {
        return goal(Chapter1Maps::Forest, Wagon);
    }
    if (!m_State.prisonersReleased)
    {
        return goal(Chapter1Maps::Forest, Prisoner);
    }
    if (!m_State.mineEvidenceAcquired)
    {
        return goal(Chapter1Maps::Forest, Invoice);
    }
    if (!m_State.grumaDuelStarted && !m_State.grumaDefeated)
    {
        if (m_State.mineEntryRoute == Chapter1MineEntryRoute::None)
        {
            return goal(Chapter1Maps::Mine, FrontGate);
        }
        return ChapterGoal{Chapter1Maps::Mine, 0, Arena()};
    }
    if (m_State.phase == Chapter1Phase::ArmyCommand)
    {
        const ObjectId bells[] = {BellA, BellB, BellC};
        for (int i = 0; i < 3; ++i)
        {
            if (m_State.bellHealth[i] > 0)
            {
                return ChapterGoal{
                    Chapter1Maps::Mine, bells[i], std::nullopt, "지휘 종 파괴", "[Ctrl] 종 공격"};
            }
        }
        return ChapterGoal{Chapter1Maps::Mine,
                           ReinforcementGate,
                           std::nullopt,
                           "증원 문 차단",
                           "[Space 유지] 문 차단"};
    }
    if (!m_State.grumaDefeated)
    {
        return ChapterGoal{Chapter1Maps::Mine, 0, Arena()};
    }
    return goal(Chapter1Maps::Mine, m_State.commandLedgerAcquired ? Hero : Ledger);
}

void Chapter2::VisitMap(MapId map)
{
    if (map != m_State.map)
    {
        m_RegionCombat.at(static_cast<std::size_t>(m_State.map - Chapter2Maps::Graywall)) =
            m_Combat;
        m_Combat = m_RegionCombat.at(static_cast<std::size_t>(map - Chapter2Maps::Graywall));
        m_State.map = map;
    }
    m_Impact = {};
    ClearTravel();
}

void Chapter2::PlayerArrived(WorldPosition player, const LevelMap& map)
{
    if (m_State.phase == Chapter2Phase::Reunion || m_Combat.finishing)
    {
        return;
    }
    if (m_Combat.heroPresent)
    {
        m_Combat.hero = SafeSpawn(player, -55.f, 20.f, map);
        m_Combat.heroPath.clear();
        m_Combat.heroRepath = 0.f;
    }
}

std::optional<ChapterGoal> Chapter2::Goal() const
{
    using namespace Chapter2Objects;
    const auto goal = [](MapId map, ObjectId object)
    {
        return ChapterGoal{map, object, std::nullopt};
    };
    const auto graywall = [](ObjectId object)
    {
        return ChapterGoal{Chapter2Maps::Graywall, object, std::nullopt};
    };
    switch (m_State.phase)
    {
    case Chapter2Phase::Briefing:
        return graywall(Briefing);
    case Chapter2Phase::Reconnaissance:
        return graywall(!Has(Chapter2Event::SiegeMismatchFound) ? Tracks : Schedule);
    case Chapter2Phase::Frontline:
        return graywall(!Has(Chapter2Event::FrontlineSupported)     ? Frontline
                        : !Has(Chapter2Event::BeaconDirectionFound) ? Signal
                                                                    : StopPursuit);
    case Chapter2Phase::ConvoyAnalysis:
        return goal(Chapter2Maps::Canyon, TacticalMap);
    case Chapter2Phase::Priorities:
        if (m_State.activeTarget)
        {
            const auto& combat = m_State.map == Chapter2Maps::Canyon ? m_Combat : m_RegionCombat[1];
            return ChapterGoal{Chapter2Maps::Canyon,
                               0,
                               DevicePosition(*m_State.activeTarget, combat.objectiveStep),
                               std::string(TargetName(*m_State.activeTarget)) + " " +
                                   std::to_string(combat.objectiveStep + 1) + "/" +
                                   std::to_string(TargetSteps(*m_State.activeTarget)),
                               *m_State.activeTarget == Chapter2Target::Camp &&
                                       combat.objectiveStep > 0
                                   ? "대피 지점에서 적 처치 후 방어"
                               : combat.cleared ? "[Space 유지] 장치 조작"
                                                : "주변 적부터 처치 [Ctrl]"};
        }
        return goal(Chapter2Maps::Canyon,
                    m_State.completedTargets == 3 ? CommandOrders : TacticalMap);
    case Chapter2Phase::Infiltration:
        return goal(Chapter2Maps::OldFortress, m_State.route ? Mural : Aqueduct);
    case Chapter2Phase::Reunion:
        if (!Has(Chapter2Event::DemolitionCut) || !Has(Chapter2Event::BypassOpened))
        {
            return ChapterGoal{Chapter2Maps::OldFortress,
                               !Has(Chapter2Event::DemolitionCut) ? DemolitionLine : Bypass,
                               std::nullopt,
                               "용사 합류로 확보",
                               "경계 처치 후 [Space 유지]"};
        }
        return goal(Chapter2Maps::OldFortress, ReunionSignal);
    case Chapter2Phase::Boss:
        if (!Has(Chapter2Event::RetreatBlocked))
        {
            const auto& bossCombat =
                m_State.map == Chapter2Maps::OldFortress ? m_Combat : m_RegionCombat[2];
            if (bossCombat.bossHealthRatio > 2.f / 3.f)
            {
                const auto position = bossCombat.bossPosition.y == 0.f ? WorldPosition{0.f, -2900.f}
                                                                       : bossCombat.bossPosition;
                return ChapterGoal{Chapter2Maps::OldFortress,
                                   0,
                                   position,
                                   "바르칸 압박",
                                   "[Ctrl] 공격 · 위험 범위 회피"};
            }
            return ChapterGoal{Chapter2Maps::OldFortress,
                               RetreatChain,
                               std::nullopt,
                               "바르칸 퇴로 차단",
                               "[Space 유지] 쇠사슬 장치"};
        }
        return ChapterGoal{Chapter2Maps::OldFortress,
                           Has(Chapter2Event::ArmorBroken) ? CommandTarget : ArmorOpening,
                           std::nullopt,
                           "용사와 합동 공격",
                           Has(Chapter2Event::ArmorBroken) ? "[Ctrl] 지휘 인장 공격"
                                                           : "바르칸을 공격해 자세 붕괴"};
    case Chapter2Phase::Aftermath:
        return goal(Chapter2Maps::Canyon,
                    Has(Chapter2Event::AftermathObserved) ? MuralReview : ReliefReport);
    case Chapter2Phase::Complete:
        return std::nullopt;
    }
    return std::nullopt;
}
