#include "stdafx.h"
#include "Chapter1.h"
#include "Chapter2.h"

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
