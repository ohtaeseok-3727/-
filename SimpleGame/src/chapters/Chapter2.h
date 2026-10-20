#pragma once

#include "Chapter.h"
#include "Chapter2Sprites.h"
#include "EnemyPathfinder.h"
#include <array>
#include <bitset>

namespace Chapter2Maps
{
    enum : MapId
    {
        Graywall = 4,
        Canyon = 5,
        OldFortress = 6
    };
}

namespace Chapter2Objects
{
    enum : ObjectId
    {
        Briefing = 1,
        Tracks,
        Schedule,
        Frontline,
        Signal,
        StopPursuit,
        CanyonExit,
        TacticalMap,
        ChooseBeacon,
        ChooseWorkshop,
        ChooseCamp,
        BeaconControl,
        WorkshopControl,
        CampControl,
        CommandOrders,
        FortressExit,
        Aqueduct,
        Chapel,
        Breach,
        Mural,
        DemolitionLine,
        Bypass,
        ReunionSignal,
        RetreatChain,
        ArmorOpening,
        CommandTarget,
        JointFinisher,
        AftermathExit,
        ReliefReport,
        MuralReview,
        PreviewGraywall,
        PreviewCanyon,
        PreviewFortress,
        BossDemolition
    };
}

enum class Chapter2Phase
{
    Briefing,
    Reconnaissance,
    Frontline,
    ConvoyAnalysis,
    Priorities,
    Infiltration,
    Reunion,
    Boss,
    Aftermath,
    Complete
};

enum class Chapter2Target
{
    Beacon,
    Workshop,
    Camp,
    Count
};

enum class Chapter2Condition
{
    Intact,
    Damaged,
    Critical
};

enum class Chapter2StageLength
{
    Full,
    HalfEmergency,
    BriefDamageControl
};

enum class Chapter2Route
{
    Aqueduct,
    Chapel,
    Breach,
    Count
};

enum class Chapter2Event
{
    BriefingHeard,
    SiegeMismatchFound,
    ScheduleRecovered,
    FrontlineSupported,
    BeaconDirectionFound,
    PursuitAbandoned,
    ConvoyPlanRead,
    CommandOrdersRecovered,
    MuralCopied,
    DemolitionCut,
    BypassOpened,
    HeroRejoined,
    RetreatBlocked,
    ArmorBroken,
    CommandTargetDestroyed,
    JointFinisherEnded,
    AftermathObserved,
    MuralReviewed,
    Count
};

// Qualitative loss levels, not casualty counts or approved balancing numbers.
struct Chapter2Outcome
{
    int beaconDelay = 0;
    int bridgeDamage = 0;
    int refugeeLoss = 0;
    int supplyLoss = 0;
    int eliteReinforcement = 0;
    bool chapelRouteInfo = false;
    bool refugeeMajoritySurvived = false;
    bool muralEvidence = false;
    bool barkanDefeated = false;
    bool equalPartnership = false;
    bool complete = false;
};

struct Chapter2State
{
    MapId map = Chapter2Maps::Graywall;
    Chapter2Phase phase = Chapter2Phase::Briefing;
    std::bitset<static_cast<std::size_t>(Chapter2Event::Count)> events;
    std::array<int, 3> priority{-1, -1, -1};
    std::array<Chapter2Condition, 3> conditions{};
    std::bitset<3> resolved;
    std::optional<Chapter2Target> activeTarget;
    std::optional<Chapter2Route> route;
    int completedTargets = 0;
    Chapter2Outcome outcome;
};

// Transient combat checkpoint. Story choices live separately and survive retries.
struct Chapter2Combat
{
    int group = 0;
    int wave = 0;
    int waves = 0;
    bool spawned = false;
    bool cleared = false;
    float interaction = 0.f;
    WorldPosition center{};
    WorldPosition hero{};
    bool heroPresent = false;
    bool reunionRequested = false;
    float heroCooldown = 0.f;
    float heroAttack = 0.f;
    float heroRepath = 0.f;
    std::size_t heroWaypoint = 0;
    bool heroFaceLeft = false;
    std::vector<WorldPosition> heroPath;
    float jumpCooldown = 0.f;
    float cryCooldown = 4.f;
    float finisher = 0.f;
    bool finishing = false;
    bool reinforcements = false;
    bool demolitionDisabled = false;
    int objectiveStep = 0;
    float defenseTime = 0.f;
    float hazardCooldown = 5.f;
    float explosionWarning = 0.f;
    WorldPosition explosionPosition{};
    WorldPosition bossPosition{};
    WorldPosition bossTarget{};
    EnemyAction bossAction = EnemyAction::None;
    float bossProgress = 0.f;
    float bossHealthRatio = 1.f;
};

// One chapter owns story, encounters and companion/boss behavior.
class Chapter2 final : public Chapter
{
  public:
    const char* Title() const override;
    MapId InitialMap() const override;
    MapDefinition BuildMap(MapId map) const override;
    void Reset() override;
    bool CanSkipFieldStory() const;
    bool SkipFieldStory();
    void EnterMap(MapId map) noexcept override;
    void VisitMap(MapId map) override;
    void PlayerArrived(WorldPosition player, const LevelMap& map) override;
    std::optional<ChapterGoal> Goal() const override;
    std::string InteractionPrompt(const MapLandmark& site) const override;
    void Update(float dt,
                WorldPosition player,
                bool interactionHeld,
                const LevelMap& map,
                EnemySystem& enemies,
                PlayerProgression& progress) override;
    std::string Interact(const MapLandmark& site,
                         const LevelMap& map,
                         EnemySystem& enemies,
                         PlayerProgression& progress,
                         PlayerInventory& inventory) override;
    void Retry(EnemySystem& enemies) override;
    void StrikeObjects(WorldPosition player,
                       bool faceLeft,
                       int damage,
                       const LevelMap& map,
                       float rangeScale = 1.f) override;
    void StrikeProjectileObjects(WorldPosition from,
                                 WorldPosition to,
                                 float radiusX,
                                 float radiusY,
                                 int damage,
                                 const LevelMap& map,
                                 float height = LevelTuning::ProjectileHeight,
                                 std::unordered_set<std::uint64_t>* hitObjects = nullptr,
                                 std::optional<WorldPosition> sightOrigin = std::nullopt) override;
    ChapterImpact ConsumeImpact() override;
    bool BlocksMovement(WorldPosition p) const override;
    bool EnemyVisible(const FieldEnemy& enemy) const override;
    void AddActors(SceneGraph& scene, WorldPosition player) const override;
    void DrawActor(Renderer& renderer, int actor, Point p, float time) const override;
    bool ControlLocked() const override;
    bool Complete() const override;
    const std::string& Message() const override;
    std::string Objective() const override;
    ChapterFlags ExportFlags() const override;
    bool DrawLandmark(Renderer& renderer,
                      const MapLandmark& site,
                      Point p,
                      float time) const override;
    void DrawEnemy(Renderer& renderer, const FieldEnemy& enemy, Point p) const override;
    void DrawWeather(Renderer& renderer, float time, int width, int height) const override;

    const Chapter2State& State() const;
    const Chapter2Combat& Combat() const;
    MonitorObservation ObserveMap(const MapDefinition& map, WorldPosition player) const override;
    bool RecordEvent(Chapter2Event event);
    bool ChooseTarget(Chapter2Target target);
    bool ResolveTarget(Chapter2Target target);
    bool SelectRoute(Chapter2Route route);
    bool RouteAvailable(Chapter2Route route) const;
    Chapter2StageLength StageLength(Chapter2Target target) const;
    static WorldPosition DevicePosition(Chapter2Target target, int step);
    int TargetSteps(Chapter2Target target) const;
    static const char* TargetName(Chapter2Target target);
    static const char* TargetRisk(Chapter2Target target);

  private:
    bool Has(Chapter2Event event) const;
    void UpdateConsequences();
    void BeginEncounter(int group, WorldPosition center, int waves);
    void UpdateEncounter(WorldPosition player, const LevelMap& map, EnemySystem& enemies);
    void UpdateHero(float dt, WorldPosition player, const LevelMap& map, EnemySystem& enemies);
    void UpdateBoss(float dt, WorldPosition player, const LevelMap& map, EnemySystem& enemies);
    bool HoldDevice(float dt,
                    bool held,
                    WorldPosition player,
                    WorldPosition device,
                    const LevelMap& map,
                    float duration);
    WorldPosition SafeSpawn(WorldPosition center, float dx, float dy, const LevelMap& map) const;
    void SpawnOrcs(EnemySystem& enemies,
                   const LevelMap& map,
                   WorldPosition center,
                   int group,
                   int count,
                   int elites);
    void DestroyCommandTarget();
    static MapDefinition GraywallMap();
    static MapDefinition CanyonMap();
    static MapDefinition FortressMap();
    Chapter2State m_State;
    std::string m_Message;
    Chapter2Combat m_Combat;
    std::array<Chapter2Combat, 3> m_RegionCombat;
    ChapterImpact m_Impact;
    EnemyPathfinder m_HeroPathfinder;
    mutable Chapter2Sprites m_Sprites;
};
