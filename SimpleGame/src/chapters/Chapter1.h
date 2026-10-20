#pragma once

#include "Chapter.h"
#include "Chapter1Sprites.h"
#include <array>
#include <bitset>

namespace Chapter1Maps
{
    enum : MapId
    {
        Village,
        Forest,
        Mine,
        Chapter2Route
    };

}

namespace Chapter1Objects
{
    enum : ObjectId
    {
        None,
        Ration,
        Donation,
        MilitarySupply,
        Notice,
        ClosedMill,
        Farmer,
        FoodEvidence,
        RepairEvidence,
        TransportEvidence,
        NorthExit,
        LightTracks,
        HeavyTracks,
        Grain,
        Bark,
        Rope,
        Ridge,
        Wagon,
        Prisoner,
        Invoice,
        MineExit,
        FrontGate,
        Drain,
        Storage,
        Training,
        Repair,
        ShiftBell,
        TransportRecords,
        CommandSeal,
        Lift,
        BellA,
        BellB,
        BellC,
        ReinforcementGate,
        Ledger,
        Hero,
        ChapterExit
    };

}

enum class Chapter1Encounter : std::size_t
{
    FarmRaid = 1,
    WrongPathAmbush,
    Convoy,
    MineGate,
    InternalPatrol,
    GrumaDuel,
    ArmyCommand,
    HeroPayoff
};

enum class Chapter1MineEntryRoute
{
    None,
    FrontGate,
    Drainage
};
enum class Chapter1Phase
{
    Exploration,
    Duel,
    ArmyCommand,
    Execution,
    HeroPayoff,
    Aftermath,
    Complete
};

struct Chapter1State
{
    MapId map = Chapter1Maps::Village;
    Chapter1Phase phase = Chapter1Phase::Exploration;
    bool rationInteractionComplete = false;
    bool reliefDonationMade = false;
    bool villageAlert = false;
    bool farmRaidResolved = false;
    std::bitset<3> investigationEvidence;
    int investigationEvidenceCount = 0;
    bool northTrailUnlocked = false;
    std::bitset<5> trackingClues;
    bool realTrackFound = false;
    bool wrongPathAmbushTriggered = false;
    bool convoySpotted = false;
    bool convoyDisabled = false;
    bool prisonersReleased = false;
    bool mineEvidenceAcquired = false;
    int suppliesPreserved = 100;
    Chapter1MineEntryRoute mineEntryRoute = Chapter1MineEntryRoute::None;
    std::bitset<6> mineInvestigation;
    bool commandSealFound = false;
    bool grumaDuelStarted = false;
    bool grumaArmorBroken = false;
    bool grumaLegInjured = false;
    std::array<int, 3> bellHealth{60, 60, 60};
    int commandBellsDestroyed = 0;
    float gateProgress = 0.f;
    bool reinforcementGateClosed = false;
    bool strategicMissionSuccess = false;
    bool playerIncapacitated = false;
    bool heroArrived = false;
    bool grumaDefeated = false;
    bool commandLedgerAcquired = false;
    bool aftermathDialogueComplete = false;
    bool chapter1Complete = false;
    std::bitset<36> triggers;
};

class Chapter1 final : public Chapter
{
  public:
    const char* Title() const override;
    MapId InitialMap() const override;
    MapDefinition BuildMap(MapId map) const override;
    void EnterMap(MapId map) noexcept override;
    void VisitMap(MapId map) override;
    std::optional<ChapterGoal> Goal() const override;
    std::string InteractionPrompt(const MapLandmark& site) const override;
    bool Complete() const override;
    bool EnemyVisible(const FieldEnemy& enemy) const override;
    void AddActors(SceneGraph& scene, WorldPosition player) const override;
    bool DrawLandmark(Renderer& renderer,
                      const MapLandmark& site,
                      Point p,
                      float time) const override;
    void DrawActor(Renderer& renderer, int actor, Point p, float time) const override;
    void DrawEnemy(Renderer& renderer, const FieldEnemy& enemy, Point p) const override;
    void DrawWeather(Renderer& renderer, float time, int width, int height) const override;
    static WorldPosition Arena();
    void Reset() override;
    void SkipStory();
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
    void Retry(EnemySystem& enemies) override;
    bool ControlLocked() const override;
    const Chapter1State& State() const;
    MonitorObservation ObserveMap(const MapDefinition& map, WorldPosition player) const override;
    const std::string& Message() const override;
    std::string Objective() const override;
    static WorldPosition Position(const LevelMap& map, ObjectId object);
    static constexpr float GateHoldSeconds = 4.f;

  private:
    static void PopulateVillage(MapDefinition& layout);
    static void PopulateForest(MapDefinition& layout);
    static void PopulateMine(MapDefinition& layout);
    static void PopulateChapter2Route(MapDefinition& layout);
    void DrawHero(Renderer& renderer, Point p, float time) const;
    bool EncounterStarted(Chapter1Encounter id) const;
    bool EncounterComplete(Chapter1Encounter id, const EnemySystem& enemies) const;
    void StartEncounter(Chapter1Encounter id,
                        WorldPosition center,
                        int count,
                        EnemySystem& enemies,
                        const LevelMap& map);
    void SpawnGroup(Chapter1Encounter id,
                    WorldPosition center,
                    int count,
                    EnemyRole role,
                    EnemySystem& enemies,
                    const LevelMap& map);
    void Reinforce(WorldPosition center,
                   int bellsDestroyed,
                   EnemySystem& enemies,
                   const LevelMap& map);
    void Fire(int trigger, const std::string& message);
    void Village(float dt, WorldPosition player, const LevelMap& map, EnemySystem& enemies);
    void Forest(float dt, WorldPosition player, const LevelMap& map, EnemySystem& enemies);
    void Mine(float dt,
              WorldPosition player,
              bool held,
              const LevelMap& map,
              EnemySystem& enemies,
              PlayerProgression& progress);
    void BossBattle(float dt,
                    WorldPosition player,
                    bool held,
                    const LevelMap& map,
                    EnemySystem& enemies,
                    PlayerProgression& progress);
    void Payoff(float dt, EnemySystem& enemies, PlayerProgression& progress);
    std::string VillageInteraction(ObjectId object, PlayerInventory& inventory);
    std::string ForestInteraction(ObjectId object, const EnemySystem& enemies);
    std::string MineInteraction(ObjectId object, const LevelMap& map);
    void Travel(MapId next);
    Chapter1State m_State;
    std::bitset<9> m_Started;
    mutable Chapter1Sprites m_Sprites;
    std::string m_Message;
    float m_AlarmTime = 0.f;
    float m_WaveTime = 0.f;
    float m_PhaseTime = 0.f;
    float m_ConvoyTime = 0.f;
};
