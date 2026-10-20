#include "stdafx.h"
#include "Chapter2.h"
#include <algorithm>

namespace
{
    bool ValidTarget(Chapter2Target target)
    {
        return target >= Chapter2Target::Beacon && target < Chapter2Target::Count;
    }
}

const char* Chapter2::Title() const
{
    return "챕터 2 · 협곡의 미끼";
}

MapId Chapter2::InitialMap() const
{
    return Chapter2Maps::Graywall;
}

void Chapter2::Reset()
{
    m_State = Chapter2State{};
    m_RegionCombat = {};
    m_Combat = Chapter2Combat{};
    m_Impact = {};
    ClearTravel();
    m_Message = "그르마의 장부가 가리킨 회색벽. 작전실에서 용사와 방어 계획을 확인하세요.";
}

void Chapter2::EnterMap(MapId map) noexcept
{
    m_State.map = map;
    m_Combat = Chapter2Combat{};
    m_Impact = {};
}

bool Chapter2::Has(Chapter2Event event) const
{
    return m_State.events.test(static_cast<std::size_t>(event));
}

bool Chapter2::RecordEvent(Chapter2Event event)
{
    if (event < Chapter2Event::BriefingHeard || event >= Chapter2Event::Count || Has(event))
    {
        return false;
    }
    const auto phase = m_State.phase;
    const auto map = m_State.map;
    switch (event)
    {
    case Chapter2Event::BriefingHeard:
        if (map != Chapter2Maps::Graywall || phase != Chapter2Phase::Briefing)
        {
            return false;
        }
        m_State.phase = Chapter2Phase::Reconnaissance;
        break;
    case Chapter2Event::SiegeMismatchFound:
    case Chapter2Event::ScheduleRecovered:
        if (map != Chapter2Maps::Graywall || phase != Chapter2Phase::Reconnaissance)
        {
            return false;
        }
        break;
    case Chapter2Event::FrontlineSupported:
        if (map != Chapter2Maps::Graywall || phase != Chapter2Phase::Frontline)
        {
            return false;
        }
        break;
    case Chapter2Event::BeaconDirectionFound:
        if (map != Chapter2Maps::Graywall || phase != Chapter2Phase::Frontline ||
            !Has(Chapter2Event::FrontlineSupported))
        {
            return false;
        }
        break;
    case Chapter2Event::PursuitAbandoned:
        if (map != Chapter2Maps::Graywall || phase != Chapter2Phase::Frontline ||
            !Has(Chapter2Event::ScheduleRecovered) || !Has(Chapter2Event::BeaconDirectionFound))
        {
            return false;
        }
        m_State.phase = Chapter2Phase::ConvoyAnalysis;
        break;
    case Chapter2Event::ConvoyPlanRead:
        if (map != Chapter2Maps::Canyon || phase != Chapter2Phase::ConvoyAnalysis)
        {
            return false;
        }
        m_State.phase = Chapter2Phase::Priorities;
        break;
    case Chapter2Event::CommandOrdersRecovered:
        if (map != Chapter2Maps::Canyon || phase != Chapter2Phase::Priorities ||
            m_State.completedTargets != 3)
        {
            return false;
        }
        m_State.phase = Chapter2Phase::Infiltration;
        break;
    case Chapter2Event::MuralCopied:
        if (map != Chapter2Maps::OldFortress || phase != Chapter2Phase::Infiltration ||
            !m_State.route)
        {
            return false;
        }
        m_State.outcome.muralEvidence = true;
        m_State.phase = Chapter2Phase::Reunion;
        break;
    case Chapter2Event::DemolitionCut:
    case Chapter2Event::BypassOpened:
        if (map != Chapter2Maps::OldFortress || phase != Chapter2Phase::Reunion)
        {
            return false;
        }
        break;
    case Chapter2Event::HeroRejoined:
        if (map != Chapter2Maps::OldFortress || phase != Chapter2Phase::Reunion ||
            !Has(Chapter2Event::DemolitionCut) || !Has(Chapter2Event::BypassOpened))
        {
            return false;
        }
        m_State.phase = Chapter2Phase::Boss;
        break;
    case Chapter2Event::RetreatBlocked:
        if (map != Chapter2Maps::OldFortress || phase != Chapter2Phase::Boss)
        {
            return false;
        }
        break;
    case Chapter2Event::ArmorBroken:
        if (map != Chapter2Maps::OldFortress || phase != Chapter2Phase::Boss ||
            !Has(Chapter2Event::RetreatBlocked))
        {
            return false;
        }
        break;
    case Chapter2Event::CommandTargetDestroyed:
        if (map != Chapter2Maps::OldFortress || phase != Chapter2Phase::Boss ||
            !Has(Chapter2Event::ArmorBroken))
        {
            return false;
        }
        break;
    case Chapter2Event::JointFinisherEnded:
        if (map != Chapter2Maps::OldFortress || phase != Chapter2Phase::Boss ||
            !Has(Chapter2Event::CommandTargetDestroyed))
        {
            return false;
        }
        m_State.outcome.barkanDefeated = true;
        m_State.outcome.bridgeDamage = (std::max)(1, m_State.outcome.bridgeDamage);
        m_State.outcome.supplyLoss = (std::max)(1, m_State.outcome.supplyLoss);
        m_State.outcome.refugeeMajoritySurvived = true;
        m_State.phase = Chapter2Phase::Aftermath;
        break;
    case Chapter2Event::AftermathObserved:
        if (map != Chapter2Maps::Canyon || phase != Chapter2Phase::Aftermath)
        {
            return false;
        }
        break;
    case Chapter2Event::MuralReviewed:
        if (map != Chapter2Maps::Canyon || phase != Chapter2Phase::Aftermath ||
            !Has(Chapter2Event::AftermathObserved) || !m_State.outcome.muralEvidence)
        {
            return false;
        }
        m_State.outcome.equalPartnership = true;
        m_State.outcome.complete = true;
        m_State.phase = Chapter2Phase::Complete;
        break;
    default:
        return false;
    }
    m_State.events.set(static_cast<std::size_t>(event));
    if (m_State.phase == Chapter2Phase::Reconnaissance && Has(Chapter2Event::SiegeMismatchFound) &&
        Has(Chapter2Event::ScheduleRecovered))
    {
        m_State.phase = Chapter2Phase::Frontline;
    }
    return true;
}

bool Chapter2::ChooseTarget(Chapter2Target target)
{
    if (!ValidTarget(target) || m_State.map != Chapter2Maps::Canyon ||
        m_State.phase != Chapter2Phase::Priorities || m_State.activeTarget ||
        m_State.resolved.test(static_cast<std::size_t>(target)))
    {
        return false;
    }
    const auto index = static_cast<std::size_t>(target);
    m_State.activeTarget = target;
    m_State.priority[index] = m_State.completedTargets;
    m_Combat.group = 0;
    m_Combat.interaction = 0.f;
    if (!ContinuousWorld())
    {
        // Legacy authored-map verification keeps its explicit travel contract.
        const auto center = DevicePosition(target, 0);
        const WorldPosition approach{center.x * .82f, center.y * .82f};
        m_Combat.hero = {approach.x - 55.f, approach.y + 20.f};
        m_Combat.heroPath.clear();
        m_Combat.heroRepath = 0.f;
        RequestTravel({{}, Chapter2Maps::Canyon, approach});
    }
    return true;
}

bool Chapter2::ResolveTarget(Chapter2Target target)
{
    if (!ValidTarget(target) || m_State.map != Chapter2Maps::Canyon ||
        m_State.phase != Chapter2Phase::Priorities || m_State.activeTarget != target ||
        m_State.resolved.test(static_cast<std::size_t>(target)))
    {
        return false;
    }
    m_State.resolved.set(static_cast<std::size_t>(target));
    m_State.activeTarget.reset();
    ++m_State.completedTargets;
    UpdateConsequences();
    return true;
}

void Chapter2::UpdateConsequences()
{
    for (std::size_t i = 0; i < m_State.conditions.size(); ++i)
    {
        if (!m_State.resolved.test(i))
        {
            m_State.conditions[i] =
                static_cast<Chapter2Condition>((std::min)(2, m_State.completedTargets));
        }
    }
    const auto delay = [this](Chapter2Target target)
    {
        const auto index = static_cast<std::size_t>(target);
        return m_State.resolved.test(index) ? m_State.priority[index] : m_State.completedTargets;
    };
    auto& outcome = m_State.outcome;
    outcome.beaconDelay = (std::min)(2, delay(Chapter2Target::Beacon));
    outcome.eliteReinforcement = outcome.beaconDelay;
    outcome.bridgeDamage = (std::min)(2, delay(Chapter2Target::Workshop));
    outcome.supplyLoss = outcome.bridgeDamage;
    outcome.refugeeLoss = (std::min)(2, delay(Chapter2Target::Camp));
    outcome.chapelRouteInfo =
        m_State.resolved.test(static_cast<std::size_t>(Chapter2Target::Camp)) &&
        m_State.priority[static_cast<std::size_t>(Chapter2Target::Camp)] == 0;
}

Chapter2StageLength Chapter2::StageLength(Chapter2Target target) const
{
    if (!ValidTarget(target))
    {
        return Chapter2StageLength::BriefDamageControl;
    }
    const auto priority = m_State.priority[static_cast<std::size_t>(target)];
    const int rank = priority >= 0 ? priority : m_State.completedTargets;
    return rank == 0   ? Chapter2StageLength::Full
           : rank == 1 ? Chapter2StageLength::HalfEmergency
                       : Chapter2StageLength::BriefDamageControl;
}

bool Chapter2::RouteAvailable(Chapter2Route route) const
{
    if (!Has(Chapter2Event::CommandOrdersRecovered))
    {
        return false;
    }
    // Provisional blockout policy. Source material does not define exact route thresholds.
    switch (route)
    {
    case Chapter2Route::Aqueduct:
        return true; // Keep one route reachable for all six order permutations.
    case Chapter2Route::Chapel:
        return m_State.outcome.chapelRouteInfo;
    case Chapter2Route::Breach:
        return m_State.outcome.bridgeDamage > 0;
    default:
        return false;
    }
}

bool Chapter2::SelectRoute(Chapter2Route route)
{
    if (m_State.map != Chapter2Maps::OldFortress || m_State.phase != Chapter2Phase::Infiltration ||
        m_State.route || !RouteAvailable(route))
    {
        return false;
    }
    m_State.route = route;
    return true;
}

const char* Chapter2::TargetName(Chapter2Target target)
{
    switch (target)
    {
    case Chapter2Target::Beacon:
        return "봉화탑";
    case Chapter2Target::Workshop:
        return "공성 작업장";
    case Chapter2Target::Camp:
        return "포로 수용소";
    default:
        return "알 수 없는 목표";
    }
}

const char* Chapter2::TargetRisk(Chapter2Target target)
{
    switch (target)
    {
    case Chapter2Target::Beacon:
        return "늦으면 바르칸 정예병이 최종전에 합류합니다.";
    case Chapter2Target::Workshop:
        return "늦으면 교량 붕괴와 겨울 보급품 손실이 커집니다.";
    case Chapter2Target::Camp:
        return "늦으면 민간 피해 증가 · 빠른 구출은 예배당 통로 정보.";
    default:
        return "";
    }
}

std::string Chapter2::Interact(const MapLandmark& site,
                               const LevelMap&,
                               EnemySystem& enemies,
                               PlayerProgression&,
                               PlayerInventory&)
{
    if (ControlLocked())
    {
        return "용사와 함께 바르칸의 지휘를 끝내고 있습니다.";
    }
    if (Complete() && site.object == Chapter2Objects::MuralReview)
    {
        if (!ContinuousWorld())
        {
            RequestTravel({"chapter3", std::nullopt, std::nullopt});
        }
        return "왕립 기록관 기념홀에서 엘리안과 공식 전승을 대조하세요. [M] 금빛 목표 확인";
    }
    std::optional<Chapter2Event> event;
    bool accepted = false;
    switch (site.object)
    {
    case Chapter2Objects::Briefing:
        event = Chapter2Event::BriefingHeard;
        break;
    case Chapter2Objects::Tracks:
        event = Chapter2Event::SiegeMismatchFound;
        break;
    case Chapter2Objects::Schedule:
        if (m_Combat.group != 201 || !m_Combat.cleared)
        {
            return "감시초소의 오크 경계를 무너뜨린 뒤 일정표를 회수하세요.";
        }
        event = Chapter2Event::ScheduleRecovered;
        break;
    case Chapter2Objects::Frontline:
        return "용사가 정면을 밀어냅니다. 전열을 함께 격파한 뒤 측면 신호병을 추적하세요.";
    case Chapter2Objects::Signal:
        if (m_Combat.group != 203 || !m_Combat.cleared)
        {
            return "측면 신호병 호위를 먼저 격파하세요. 봉화는 왕의 협곡을 향하고 있습니다.";
        }
        event = Chapter2Event::BeaconDirectionFound;
        break;
    case Chapter2Objects::StopPursuit:
        event = Chapter2Event::PursuitAbandoned;
        break;
    case Chapter2Objects::TacticalMap:
        event = Chapter2Event::ConvoyPlanRead;
        break;
    case Chapter2Objects::CommandOrders:
        event = Chapter2Event::CommandOrdersRecovered;
        break;
    case Chapter2Objects::Mural:
        if (m_Combat.group != 220 || !m_Combat.cleared)
        {
            return "선택한 진입로의 경계를 통과해 벽화를 조사하세요.";
        }
        return "벽화 옆에서 Space를 10초 유지해 훼손 흔적과 탁본을 확보하세요.";
    case Chapter2Objects::DemolitionLine:
    case Chapter2Objects::Bypass:
        return "우회로 경계를 격파한 뒤 장치 옆에서 Space를 누르고 유지하세요.";
    case Chapter2Objects::ReunionSignal:
        if (m_State.phase == Chapter2Phase::Reunion && Has(Chapter2Event::DemolitionCut) &&
            Has(Chapter2Event::BypassOpened))
        {
            if (!m_Combat.reunionRequested)
            {
                m_Combat.heroPath.clear();
                m_Combat.heroWaypoint = 0;
                m_Combat.heroRepath = 0.f;
            }
            m_Combat.reunionRequested = true;
            return "플레이어: 길 열었어. 이쪽으로! 용사가 포위선을 벗어나 합류합니다.";
        }
        return "폭파선을 끊고 우회문을 열어야 용사가 전열에서 이탈할 수 있습니다.";
    case Chapter2Objects::BossDemolition:
        return "Space를 유지해 폭파 장치를 차단하세요. 작업장 대응이 늦었을수록 폭발 간격이 짧습니다.";
    case Chapter2Objects::RetreatChain:
        return "바르칸을 압박한 뒤 이 장치 옆에서 Space를 유지해 퇴로를 봉쇄하세요.";
    case Chapter2Objects::ArmorOpening:
        return "퇴로를 막고 바르칸의 체력을 낮추세요. 용사가 접근해 자세를 무너뜨립니다.";
    case Chapter2Objects::CommandTarget:
        return Has(Chapter2Event::ArmorBroken)
                   ? "노출된 지휘 인장을 직접 공격하세요. [Ctrl / 공격 스킬]"
                   : "지휘 인장이 갑옷에 보호되어 있습니다. 먼저 퇴로를 봉쇄하고 용사와 바르칸을 압박하세요.";
    case Chapter2Objects::JointFinisher:
        return "용사만으로 전투가 끝나지 않습니다. 노출된 지휘 인장을 플레이어가 파괴해야 합니다.";
    case Chapter2Objects::ReliefReport:
        event = Chapter2Event::AftermathObserved;
        break;
    case Chapter2Objects::MuralReview:
        event = Chapter2Event::MuralReviewed;
        break;
    case Chapter2Objects::ChooseBeacon:
        accepted = ChooseTarget(Chapter2Target::Beacon);
        break;
    case Chapter2Objects::ChooseWorkshop:
        accepted = ChooseTarget(Chapter2Target::Workshop);
        break;
    case Chapter2Objects::ChooseCamp:
        accepted = ChooseTarget(Chapter2Target::Camp);
        break;
    case Chapter2Objects::BeaconControl:
    case Chapter2Objects::WorkshopControl:
    case Chapter2Objects::CampControl:
        return "위험을 확인한 뒤 대응할 지역으로 접근하세요. 경계를 격파하고 장치 옆에서 Space를 유지하세요.";
    case Chapter2Objects::Aqueduct:
        accepted = SelectRoute(Chapter2Route::Aqueduct);
        break;
    case Chapter2Objects::Chapel:
        accepted = SelectRoute(Chapter2Route::Chapel);
        break;
    case Chapter2Objects::Breach:
        accepted = SelectRoute(Chapter2Route::Breach);
        break;
    case Chapter2Objects::CanyonExit:
        accepted = Has(Chapter2Event::PursuitAbandoned);
        if (accepted && !ContinuousWorld())
        {
            RequestTravel({{}, Chapter2Maps::Canyon, std::nullopt});
        }
        break;
    case Chapter2Objects::FortressExit:
        accepted = Has(Chapter2Event::CommandOrdersRecovered);
        if (accepted && !ContinuousWorld())
        {
            RequestTravel({{}, Chapter2Maps::OldFortress, std::nullopt});
        }
        break;
    case Chapter2Objects::AftermathExit:
        accepted = m_State.outcome.barkanDefeated;
        if (accepted && !ContinuousWorld())
        {
            RequestTravel({{}, Chapter2Maps::Canyon, WorldPosition{850.f, 250.f}});
        }
        break;
    case Chapter2Objects::PreviewGraywall:
    case Chapter2Objects::PreviewCanyon:
    case Chapter2Objects::PreviewFortress:
        return "진행 조건을 충족한 연결로를 이용하세요.";
    default:
        return site.description;
    }
    if (event)
    {
        accepted = RecordEvent(*event);
    }
    if (event == Chapter2Event::PursuitAbandoned && accepted)
    {
        enemies.Clear();
    }
    m_Message = accepted ? site.name + " · " + Objective()
                         : "선행 조건이 충족되지 않았거나 이미 처리한 항목입니다. " + Objective();
    return site.description + "\n" + m_Message;
}

void Chapter2::Retry(EnemySystem& enemies)
{
    ClearTravel();
    enemies.Clear();
    m_Combat = Chapter2Combat{};
    m_Impact = {};
    if (m_State.phase == Chapter2Phase::Boss &&
        (!ContinuousWorld() || m_State.map == Chapter2Maps::OldFortress))
    {
        for (const auto event : {Chapter2Event::RetreatBlocked,
                                 Chapter2Event::ArmorBroken,
                                 Chapter2Event::CommandTargetDestroyed})
        {
            m_State.events.reset(static_cast<std::size_t>(event));
        }
    }
    m_Message = "현재 전투를 다시 준비합니다. 조사·완료한 목표·선택 순서는 유지됩니다.";
}

bool Chapter2::ControlLocked() const
{
    return m_Combat.finishing;
}

bool Chapter2::Complete() const
{
    return m_State.outcome.complete;
}

const std::string& Chapter2::Message() const
{
    return m_Message;
}

const Chapter2State& Chapter2::State() const
{
    return m_State;
}

std::string Chapter2::Objective() const
{
    const auto& combat =
        ContinuousWorld() && m_State.map != Chapter2Maps::Canyon ? m_RegionCombat[1] : m_Combat;
    switch (m_State.phase)
    {
    case Chapter2Phase::Briefing:
        return "회색벽 작전실에서 브리핑 확인";
    case Chapter2Phase::Reconnaissance:
        return "성벽 흔적과 정찰병 일정표 조사";
    case Chapter2Phase::Frontline:
        return "정면 지원 전투 → 측면 신호병 추적 → 봉화 확인 → 추격 중지";
    case Chapter2Phase::ConvoyAnalysis:
        return "협곡 관측소에서 장부·일정표·봉화 방향 조합";
    case Chapter2Phase::Priorities:
        if (m_State.activeTarget)
        {
            const auto target = *m_State.activeTarget;
            const char* action = target == Chapter2Target::Beacon     ? "신호 장치 차단"
                                 : target == Chapter2Target::Workshop ? "폭약 해체·지지대 확보"
                                 : combat.objectiveStep == 0          ? "수용소 문 해제"
                                                                      : "대피 지점 방어";
            return std::string(TargetName(target)) + " · " + action + " " +
                   std::to_string(combat.objectiveStep + 1) + "/" +
                   std::to_string(TargetSteps(target)) + " · 표시된 위치에서 Space 유지/방어";
        }
        return m_State.completedTargets == 3
                   ? "명령서 확인 후 옛 성채로 이동"
                   : "위험을 읽고 봉화탑·작업장·수용소 중 다음 목적지로 이동";
    case Chapter2Phase::Infiltration:
        return "사용 가능한 진입로 선택 후 벽화 탁본 확보";
    case Chapter2Phase::Reunion:
        return "폭파선과 우회문 처리 후 용사에게 합류 신호";
    case Chapter2Phase::Boss:
        return Has(Chapter2Event::ArmorBroken)
                   ? "노출된 지휘 인장을 직접 공격 [Ctrl / 공격 스킬]"
                   : "바르칸 압박 → 쇠사슬 Space 유지 → 용사 자세 붕괴 → 지휘 인장 공격";
    case Chapter2Phase::Aftermath:
        return "협곡 구호지에서 피해 확인 후 벽화 탁본 검토";
    case Chapter2Phase::Complete:
        return "챕터 2 완료 · 챕터 3: 왕립 기록관 기념홀에서 공식 전승과 원본 기록 대조";
    }
    return {};
}

ChapterFlags Chapter2::ExportFlags() const
{
    const auto& outcome = m_State.outcome;
    return {{"C02_BEACON_DELAY", outcome.beaconDelay},
            {"C02_BRIDGE_DAMAGE", outcome.bridgeDamage},
            {"C02_REFUGEE_LOSS", outcome.refugeeLoss},
            {"C02_SUPPLY_LOSS", outcome.supplyLoss},
            {"C02_ELITE_REINFORCEMENT", outcome.eliteReinforcement},
            {"C02_CHAPEL_ROUTE_INFO", outcome.chapelRouteInfo},
            {"C02_MURAL_EVIDENCE", outcome.muralEvidence},
            {"C02_REFUGEE_MAJORITY_SURVIVED", outcome.refugeeMajoritySurvived},
            {"C02_BARKAN_DEFEATED", outcome.barkanDefeated},
            {"C02_EQUAL_PARTNERSHIP", outcome.equalPartnership},
            {"C02_COMPLETE", outcome.complete},
            {"C02_DEVELOPMENT_PREVIEW", 0}};
}
