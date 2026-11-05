#include "stdafx.h"
#include "SaveArchive.h"

void SaveArchive::Bytes(char* data, std::size_t size)
{
    if (m_Writing)
    {
        m_Stream.write(data, static_cast<std::streamsize>(size));
    }
    else
    {
        m_Stream.read(data, static_cast<std::streamsize>(size));
    }
    if (!m_Stream)
    {
        throw std::runtime_error("Truncated save data");
    }
}

void SaveArchive::Field(std::string& value)
{
    std::uint32_t size = static_cast<std::uint32_t>(value.size());
    Field(size);
    if (size > 65536)
    {
        throw std::runtime_error("Save text too long");
    }
    if (!m_Writing)
    {
        value.resize(size);
    }
    if (size)
    {
        Bytes(&value[0], size);
    }
}

void SaveArchive::Field(std::mt19937& value)
{
    std::string text;
    if (m_Writing)
    {
        std::ostringstream stream;
        stream << value;
        text = stream.str();
    }
    Field(text);
    if (!m_Writing)
    {
        std::istringstream stream(text);
        if (!(stream >> value))
        {
            throw std::runtime_error("Invalid save random state");
        }
    }
}

void SaveArchive::Fields(WorldPosition& v)
{
    (*this)(v.x, v.y);
}

void SaveArchive::Fields(EnemyAIState& v)
{
    (*this)(v.state,
            v.path,
            v.waypoint,
            v.repathTimer,
            v.mapRevision,
            v.home,
            v.patrolTarget,
            v.lastSeen,
            v.perceptionTimer,
            v.lostSightTime,
            v.patrolWait,
            v.patrolStep,
            v.alerted,
            v.returning);
    if (static_cast<int>(v.state) < 0 || v.state > EnemyState::Dead)
    {
        throw std::runtime_error("Invalid saved enemy state");
    }
}

void SaveArchive::Fields(EnemyStats& v)
{
    (*this)(v.health,
            v.damage,
            v.speed,
            v.injuredSpeed,
            v.detectionRange,
            v.minimumHealth,
            v.preserveCorpse,
            v.appearance,
            v.scriptedAI);
}

void SaveArchive::Fields(FieldEnemy& v)
{
    (*this)(v.id,
            v.ai,
            v.position,
            v.health,
            v.maxHealth,
            v.cooldown,
            v.windup,
            v.respawn,
            v.hitFlash,
            v.encounter,
            v.role,
            v.stats,
            v.animationTime,
            v.faceLeft,
            v.armorBroken,
            v.legInjured,
            v.stateTime,
            v.action,
            v.actionTime,
            v.actionDuration,
            v.actionTarget,
            v.actionOrigin,
            v.attackSpeedTime);
    if (v.id == 0 || v.maxHealth <= 0 || v.health < 0 || v.health > v.maxHealth ||
        static_cast<int>(v.role) < 0 || v.role > EnemyRole::Boss ||
        static_cast<int>(v.action) < 0 || v.action > EnemyAction::BattleCry)
    {
        throw std::runtime_error("Invalid saved enemy");
    }
}

void SaveArchive::Fields(LootDrop& v)
{
    (*this)(v.position, v.kind, v.value, v.age, v.attracted);
    if (static_cast<int>(v.kind) < 0 || v.kind > LootKind::Health || v.value < 0 || v.age < 0.f)
    {
        throw std::runtime_error("Invalid saved loot");
    }
}

void SaveArchive::Fields(ChapterTravel& v)
{
    (*this)(v.chapter, v.map, v.destination);
}

void SaveArchive::Fields(ChapterImpact& v)
{
    (*this)(v.damage, v.push);
}

void SaveArchive::Fields(Equipment& v)
{
    (*this)(v.name, v.attack);
}

void SaveArchive::Fields(Chapter1State& v)
{
    (*this)(v.map,
            v.phase,
            v.rationInteractionComplete,
            v.reliefDonationMade,
            v.villageAlert,
            v.farmRaidResolved,
            v.investigationEvidence,
            v.investigationEvidenceCount,
            v.northTrailUnlocked,
            v.trackingClues,
            v.realTrackFound,
            v.wrongPathAmbushTriggered,
            v.convoySpotted,
            v.convoyDisabled,
            v.prisonersReleased,
            v.mineEvidenceAcquired,
            v.suppliesPreserved,
            v.mineEntryRoute,
            v.mineInvestigation,
            v.commandSealFound,
            v.grumaDuelStarted,
            v.grumaArmorBroken,
            v.grumaLegInjured,
            v.bellHealth,
            v.commandBellsDestroyed,
            v.gateProgress,
            v.reinforcementGateClosed,
            v.strategicMissionSuccess,
            v.playerIncapacitated,
            v.heroArrived,
            v.grumaDefeated,
            v.commandLedgerAcquired,
            v.aftermathDialogueComplete,
            v.chapter1Complete,
            v.triggers);
    if (v.map < Chapter1Maps::Village || v.map > Chapter1Maps::Chapter2Route ||
        static_cast<int>(v.phase) < 0 || v.phase > Chapter1Phase::Complete ||
        v.commandBellsDestroyed < 0 || v.commandBellsDestroyed > 3)
    {
        throw std::runtime_error("Invalid saved chapter 1 state");
    }
}

void SaveArchive::Fields(Chapter2Outcome& v)
{
    (*this)(v.beaconDelay,
            v.bridgeDamage,
            v.refugeeLoss,
            v.supplyLoss,
            v.eliteReinforcement,
            v.chapelRouteInfo,
            v.refugeeMajoritySurvived,
            v.muralEvidence,
            v.barkanDefeated,
            v.equalPartnership,
            v.complete);
}

void SaveArchive::Fields(Chapter2State& v)
{
    (*this)(v.map,
            v.phase,
            v.events,
            v.priority,
            v.conditions,
            v.resolved,
            v.activeTarget,
            v.route,
            v.completedTargets,
            v.outcome);
    if (v.map < Chapter2Maps::Graywall || v.map > Chapter2Maps::OldFortress ||
        static_cast<int>(v.phase) < 0 || v.phase > Chapter2Phase::Complete ||
        v.completedTargets < 0 || v.completedTargets > 3 ||
        (v.activeTarget &&
         (static_cast<int>(*v.activeTarget) < 0 || *v.activeTarget >= Chapter2Target::Count)) ||
        (v.route && (static_cast<int>(*v.route) < 0 || *v.route >= Chapter2Route::Count)))
    {
        throw std::runtime_error("Invalid saved chapter 2 state");
    }
    for (int priority : v.priority)
    {
        if (priority < -1 || priority > 2)
        {
            throw std::runtime_error("Invalid saved priority");
        }
    }
}

void SaveArchive::Fields(Chapter2Combat& v)
{
    (*this)(v.group,
            v.wave,
            v.waves,
            v.spawned,
            v.cleared,
            v.interaction,
            v.center,
            v.hero,
            v.heroPresent,
            v.reunionRequested,
            v.heroCooldown,
            v.heroAttack,
            v.heroRepath,
            v.heroWaypoint,
            v.heroFaceLeft,
            v.heroMoving,
            v.heroPath,
            v.jumpCooldown,
            v.cryCooldown,
            v.finisher,
            v.finishing,
            v.reinforcements,
            v.demolitionDisabled,
            v.objectiveStep,
            v.defenseTime,
            v.hazardCooldown,
            v.explosionWarning,
            v.explosionPosition,
            v.bossPosition,
            v.bossTarget,
            v.bossAction,
            v.bossProgress,
            v.bossHealthRatio);
}

void SaveArchive::Fields(Chapter3State& v)
{
    (*this)(v.map,
            v.phase,
            v.memorial,
            v.petition,
            v.rations,
            v.shortcut,
            v.manuscripts,
            v.foundations,
            v.stoneLedger,
            v.monastery,
            v.edricHeard,
            v.movementList,
            v.storageDate,
            v.wheelsStopped,
            v.brake,
            v.seradAlive,
            v.original,
            v.notice,
            v.copies,
            v.extraCopy,
            v.testimony,
            v.hearing,
            v.restrictions,
            v.supportClaimed,
            v.previousComplete,
            v.supplyLoss,
            v.refugeeLoss);
    if (v.map < Chapter3Maps::Memorial || v.map > Chapter3Maps::ReadingRoom ||
        static_cast<int>(v.phase) < 0 || v.phase > Chapter3Phase::Complete || v.manuscripts < 0 ||
        v.manuscripts > 3 || v.monastery < 0 || v.monastery > 3 ||
        (v.extraCopy && (*v.extraCopy < 0 || *v.extraCopy > 1)))
    {
        throw std::runtime_error("Invalid saved chapter 3 state");
    }
}

void SaveArchive::Fields(PlayerProgression& v)
{
    (*this)(
        v.m_Profession, v.m_Level, v.m_Experience, v.m_Health, v.m_Mana, v.m_ManaRecoveryRemainder);
}

void SaveArchive::Fields(PlayerInventory& v)
{
    (*this)(v.m_Gold, v.m_Equipped, v.m_Items, v.m_HealthPotions, v.m_ManaPotions);
}

void SaveArchive::Fields(SkillQuickSlots& v)
{
    (*this)(v.m_Slots);
}

void SaveArchive::Fields(SkillSystem& v)
{
    (*this)(v.m_QuickSlots, v.m_Cooldowns);
}

void SaveArchive::Fields(EnemySystem& v)
{
    (*this)(v.m_UpdateStart, v.m_SupportEnabled, v.m_Enemies, v.m_ScriptDefeats, v.m_NextId);
}

void SaveArchive::Fields(LootSystem& v)
{
    (*this)(v.m_Random, v.m_Drops);
}

void SaveArchive::Fields(Chapter& v)
{
    (*this)(v.m_ContinuousWorld, v.m_Travel);
}

void SaveArchive::Fields(Chapter1& v)
{
    Field(static_cast<Chapter&>(v));
    (*this)(v.m_State,
            v.m_Started,
            v.m_Message,
            v.m_AlarmTime,
            v.m_WaveTime,
            v.m_PhaseTime,
            v.m_ConvoyTime);
}

void SaveArchive::Fields(Chapter2& v)
{
    Field(static_cast<Chapter&>(v));
    (*this)(v.m_State, v.m_Message, v.m_Combat, v.m_RegionCombat, v.m_Impact);
}

void SaveArchive::Fields(Chapter3& v)
{
    Field(static_cast<Chapter&>(v));
    (*this)(v.m_State,
            v.m_Impact,
            v.m_Message,
            v.m_Courier,
            v.m_CourierLeg,
            v.m_Hold,
            v.m_HoldAction,
            v.m_PatrolTime,
            v.m_Subdued,
            v.m_Alarm,
            v.m_CartDistance,
            v.m_AttackTime,
            v.m_Recovery,
            v.m_AttackNumber,
            v.m_AttackTarget,
            v.m_BossPosition,
            v.m_Ranged,
            v.m_WasHeld,
            v.m_ResetCombat,
            v.m_ParryWindow,
            v.m_ParryCooldown,
            v.m_Delivery);
    if (v.m_CourierLeg < 0 || v.m_CourierLeg > 3)
    {
        throw std::runtime_error("Invalid saved courier route");
    }
}

void SaveArchive::Fields(FieldPopulation& v)
{
    Field(v.m_Elapsed);
    std::uint32_t size = static_cast<std::uint32_t>(v.m_Packs.size());
    Field(size);
    if (size > 64)
    {
        throw std::runtime_error("Invalid pack count");
    }
    if (!m_Writing)
    {
        v.m_Packs.resize(size);
    }
    for (auto& p : v.m_Packs)
    {
        (*this)(p.positions, p.encounter, p.cooldown, p.health);
    }
}

void SaveArchive::Fields(GameSession& v)
{
    (*this)(v.m_Profession,
            v.m_Kills,
            v.m_Invulnerable,
            v.m_Push,
            v.m_ActiveChapterId,
            v.m_Region,
            v.m_Checkpoint,
            v.m_StartAtChapter2,
            v.m_StartAtChapter3,
            v.m_Progress,
            v.m_Inventory,
            v.m_Enemies,
            v.m_Loot);
    Field(dynamic_cast<Chapter1&>(*v.m_Chapters.at("chapter1").instance));
    Field(dynamic_cast<Chapter2&>(*v.m_Chapters.at("chapter2").instance));
    Field(dynamic_cast<Chapter3&>(*v.m_Chapters.at("chapter3").instance));
    std::uint32_t size = static_cast<std::uint32_t>(v.m_Regions.size());
    Field(size);
    if (size != v.m_Regions.size())
    {
        throw std::runtime_error("Incompatible world region count");
    }
    for (std::size_t i = 0; i < v.m_Regions.size(); ++i)
    {
        auto& region = v.m_Regions[i];
        (*this)(region.enemies, region.loot, region.population, v.m_Atlas.m_Regions[i].discovered);
    }
    if (v.m_Region >= v.m_Regions.size() ||
        v.m_ActiveChapterId != v.m_Atlas.m_Regions[v.m_Region].chapter)
    {
        throw std::runtime_error("Invalid saved region");
    }
    if (v.m_Profession < 0 || v.m_Profession > 1 || v.m_Progress.m_Profession != v.m_Profession ||
        v.m_Progress.m_Level < 1 || v.m_Progress.m_Level > 10000 || v.m_Progress.m_Experience < 0 ||
        v.m_Progress.m_Experience >= v.m_Progress.RequiredExperience() ||
        v.m_Progress.m_Health <= 0 || v.m_Progress.m_Health > v.m_Progress.MaxHealth() ||
        v.m_Progress.m_Mana < 0 || v.m_Progress.m_Mana > v.m_Progress.MaxMana() ||
        v.m_Progress.m_ManaRecoveryRemainder < 0.0 || v.m_Progress.m_ManaRecoveryRemainder >= 1.0 ||
        v.m_Inventory.m_Gold < 0 || v.m_Inventory.m_HealthPotions < 0 ||
        v.m_Inventory.m_ManaPotions < 0 || v.m_Inventory.m_Equipped < -1 ||
        v.m_Inventory.m_Equipped >= static_cast<int>(v.m_Inventory.m_Items.size()))
    {
        throw std::runtime_error("Invalid saved player values");
    }
    for (const auto& item : v.m_Inventory.m_Items)
    {
        if (item.attack < 1 || item.attack > 100000 ||
            (item.name != "철검" && item.name != "룬 장갑"))
        {
            throw std::runtime_error("Invalid saved equipment");
        }
    }
    v.m_Chapter = v.m_Chapters.at(v.m_ActiveChapterId).instance.get();
}
