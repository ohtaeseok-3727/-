#include "stdafx.h"
#include "Chapter1.h"
#include "Chapter2.h"
#include "Chapter3.h"
#include <algorithm>

void Chapter1::SkipStory()
{
    if (Complete())
    {
        return;
    }
    m_State.rationInteractionComplete = true;
    m_State.villageAlert = true;
    m_State.farmRaidResolved = true;
    m_State.investigationEvidence.set();
    m_State.investigationEvidenceCount = 3;
    m_State.northTrailUnlocked = true;
    m_State.trackingClues.set();
    m_State.realTrackFound = true;
    m_State.convoySpotted = true;
    m_State.convoyDisabled = true;
    m_State.prisonersReleased = true;
    m_State.mineEvidenceAcquired = true;
    m_State.commandSealFound = true;
    m_State.grumaDuelStarted = true;
    m_State.grumaArmorBroken = true;
    m_State.grumaLegInjured = true;
    m_State.bellHealth.fill(0);
    m_State.commandBellsDestroyed = 3;
    m_State.gateProgress = GateHoldSeconds;
    m_State.reinforcementGateClosed = true;
    m_State.strategicMissionSuccess = true;
    m_State.playerIncapacitated = false;
    m_State.heroArrived = true;
    m_State.grumaDefeated = true;
    m_State.commandLedgerAcquired = true;
    m_State.aftermathDialogueComplete = true;
    m_State.chapter1Complete = true;
    m_State.phase = Chapter1Phase::Complete;
    m_State.triggers.set();
    m_Started.set();
    m_AlarmTime = m_WaveTime = m_PhaseTime = m_ConvoyTime = 0.f;
    ClearTravel();
    m_Message = "챕터 1 스킵 완료 · 회색벽에서 다음 이야기를 시작하세요.";
}

bool Chapter2::CanSkipFieldStory() const
{
    return m_State.phase < Chapter2Phase::Infiltration;
}

bool Chapter2::SkipFieldStory()
{
    if (!CanSkipFieldStory())
    {
        return false;
    }
    const auto previousMap = m_State.map;
    // Fill only field events; fortress route, mural, reunion and boss remain unplayed.
    for (const auto event : {Chapter2Event::BriefingHeard,
                             Chapter2Event::SiegeMismatchFound,
                             Chapter2Event::ScheduleRecovered,
                             Chapter2Event::FrontlineSupported,
                             Chapter2Event::BeaconDirectionFound,
                             Chapter2Event::PursuitAbandoned,
                             Chapter2Event::ConvoyPlanRead})
    {
        m_State.events.set(static_cast<std::size_t>(event));
    }
    m_State.map = Chapter2Maps::Canyon;
    m_State.phase = Chapter2Phase::Priorities;
    if (m_State.activeTarget)
    {
        ResolveTarget(*m_State.activeTarget);
    }
    // Provisional skip order only; preserve all choices already made by the player.
    for (const auto target :
         {Chapter2Target::Beacon, Chapter2Target::Workshop, Chapter2Target::Camp})
    {
        if (ChooseTarget(target))
        {
            ResolveTarget(target);
        }
    }
    RecordEvent(Chapter2Event::CommandOrdersRecovered);
    m_State.map = previousMap;
    m_RegionCombat = {};
    m_Combat = {};
    m_Impact = {};
    ClearTravel();
    m_Message = "회색벽·협곡 스킵 완료 · 성채 진입로를 선택하세요.";
    return true;
}

bool Chapter2::SkipStory()
{
    if (Complete())
    {
        return false;
    }
    if (CanSkipFieldStory())
    {
        SkipFieldStory();
    }
    if (!m_State.route)
    {
        m_State.route = Chapter2Route::Aqueduct;
    }
    m_State.events.set();
    auto& outcome = m_State.outcome;
    outcome.muralEvidence = true;
    outcome.barkanDefeated = true;
    outcome.bridgeDamage = (std::max)(1, outcome.bridgeDamage);
    outcome.supplyLoss = (std::max)(1, outcome.supplyLoss);
    outcome.refugeeMajoritySurvived = true;
    outcome.equalPartnership = true;
    outcome.complete = true;
    m_State.phase = Chapter2Phase::Complete;
    m_RegionCombat = {};
    m_Combat = {};
    m_Impact = {};
    ClearTravel();
    m_Message = "챕터 2 스킵 완료 · 왕립 기록관 기념홀에서 다음 이야기를 시작하세요.";
    return true;
}

bool Chapter3::SkipStory()
{
    if (!m_State.previousComplete || Complete())
    {
        return false;
    }
    m_State.memorial.set();
    m_State.petition = m_State.rations = m_State.shortcut = true;
    m_State.manuscripts = 3;
    m_State.foundations.set();
    m_State.stoneLedger = true;
    m_State.monastery = 3;
    m_State.edricHeard = m_State.movementList = m_State.storageDate = true;
    m_State.wheelsStopped = m_State.brake = m_State.seradAlive = m_State.original = true;
    m_State.notice.set();
    m_State.copies.set();
    if (!m_State.extraCopy)
    {
        // Skip-only default; preserve an extra custodian already chosen by the player.
        m_State.extraCopy = 0;
    }
    m_State.testimony = m_State.hearing = true;
    m_State.restrictions.set();
    // Consume the one-time support claim without awarding skipped supplies or gold.
    m_State.supportClaimed = true;
    m_State.phase = Chapter3Phase::Complete;
    m_Impact = {};
    m_Hold = m_PatrolTime = m_Alarm = m_CartDistance = m_AttackTime = 0.f;
    m_HoldAction = m_AttackNumber = 0;
    m_Subdued.set();
    m_Ranged = m_WasHeld = m_ResetCombat = false;
    m_ParryWindow = m_ParryCooldown = 0.f;
    m_Delivery.fill(4.f);
    ClearTravel();
    m_Message = "챕터 3 스킵 완료 · 제한 현장 조사 승인. 다음 단서: 야수족 영역의 원정 좌표.";
    return true;
}
