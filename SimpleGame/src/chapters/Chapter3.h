#pragma once

#include "Chapter.h"
#include <array>
#include <bitset>

namespace Chapter3Maps
{
    constexpr MapId Memorial = 7;
    constexpr MapId Argen = 8;
    constexpr MapId Archive = 9;
}

namespace Chapter3Objects
{
    enum : ObjectId
    {
        Ceremony = 301,
        Plaque,
        Rubbing,
        CityExit,
        Petition,
        Rations,
        Permit,
        Shortcut,
        ManuscriptEarly,
        ManuscriptMiddle,
        ManuscriptLate,
        FoundationWest,
        FoundationCenter,
        FoundationEast,
        WorkshopLedger,
        RefugeRoom,
        FuneralRegister,
        LastSermon,
        Censure,
        Edric,
        TransferSeal,
        ArchiveExit,
        MovementList,
        StorageDate,
        WrongDateBox,
        WrongSupplyBox,
        OriginalBox,
        Brake,
        Disarm,
        RecoverOriginal,
        CityReturn,
        ConfirmedNotice,
        OpenNotice,
        CopyCommunity,
        CopySurvivors,
        CopyArchive,
        ExtraClergy,
        ExtraGuild,
        Testimony,
        Hearing,
        RestrictedDoor,
        ElianDesk,
        SealedWagon,
        SupplyDesk,
        CommandSeal,
        FieldPermit
    };
}

enum class Chapter3Phase
{
    Memorial,
    City,
    Chase,
    Research,
    Censure,
    Edric,
    Infiltration,
    Interception,
    Preservation,
    Hearing,
    Complete
};

// Chapter-local state. Names, coordinates, timing and combat values are implementation choices.
struct Chapter3State
{
    MapId map = Chapter3Maps::Memorial;
    Chapter3Phase phase = Chapter3Phase::Memorial;
    std::bitset<3> memorial;
    bool petition = false;
    bool rations = false;
    bool shortcut = false;
    int manuscripts = 0;
    std::bitset<3> foundations;
    bool stoneLedger = false;
    int monastery = 0;
    bool edricHeard = false;
    bool movementList = false;
    bool storageDate = false;
    bool wheelsStopped = false;
    bool brake = false;
    bool seradAlive = false;
    bool original = false;
    std::bitset<2> notice;
    std::bitset<3> copies;
    std::optional<int> extraCopy;
    bool testimony = false;
    bool hearing = false;
    std::bitset<5> restrictions;
    bool supportClaimed = false;
    bool previousComplete = false;
    int supplyLoss = 0;
    int refugeeLoss = 0;
};

class Chapter3 final : public Chapter
{
  public:
    const char* Title() const override;
    MapId InitialMap() const override;
    MapDefinition BuildMap(MapId map) const override;
    void Reset() override;
    bool SkipStory();
    void EnterMap(MapId map) noexcept override;
    void ImportPrevious(const ChapterFlags& flags) override;
    void Update(float dt,
                WorldPosition player,
                bool held,
                const LevelMap& map,
                EnemySystem& enemies,
                PlayerProgression& progress) override;
    std::string Interact(const MapLandmark& site,
                         const LevelMap& map,
                         EnemySystem& enemies,
                         PlayerProgression& progress,
                         PlayerInventory& inventory) override;
    void Retry(EnemySystem& enemies) override;
    ChapterImpact ConsumeImpact() override;
    bool ControlLocked() const override;
    bool Complete() const override;
    const std::string& Message() const override;
    std::string Objective() const override;
    std::optional<ChapterGoal> Goal() const override;
    std::string InteractionPrompt(const MapLandmark& site) const override;
    ChapterFlags ExportFlags() const override;
    void AddActors(SceneGraph& scene, WorldPosition player) const override;
    bool DrawLandmark(Renderer& renderer,
                      const MapLandmark& site,
                      Point p,
                      float time) const override;
    void DrawActor(Renderer& renderer, int actor, Point p, float time) const override;
    void DrawEnemy(Renderer& renderer, const FieldEnemy& enemy, Point p) const override;
    MonitorObservation ObserveMap(const MapDefinition& map, WorldPosition player) const override;

  private:
    static constexpr std::size_t SeradEncounter = 301;
    void Advance(Chapter3Phase phase, const char* message);
    void UpdateResearch(float dt, WorldPosition player, bool held, const LevelMap& map);
    void UpdateInfiltration(float dt, WorldPosition player, bool held, const LevelMap& map);
    void UpdateInterception(
        float dt, WorldPosition player, bool held, const LevelMap& map, EnemySystem& enemies);
    bool Hold(float dt,
              bool held,
              WorldPosition player,
              WorldPosition target,
              const LevelMap& map,
              int action,
              float seconds);
    void ResetInterception(EnemySystem& enemies);
    static MapDefinition MemorialMap();
    static MapDefinition CityMap();
    static MapDefinition ArchiveMap();
    static WorldPosition Foundation(int index);
    static WorldPosition Patrol(int index, float time);
    WorldPosition Cart() const;
    Chapter3State m_State;
    ChapterImpact m_Impact;
    std::string m_Message;
    WorldPosition m_Courier{360.f, -250.f};
    int m_CourierLeg = 0;
    float m_Hold = 0.f;
    int m_HoldAction = 0;
    float m_PatrolTime = 0.f;
    std::bitset<2> m_Subdued;
    float m_Alarm = 0.f;
    float m_CartDistance = 0.f;
    float m_AttackTime = 0.f;
    float m_Recovery = 1.f;
    int m_AttackNumber = 0;
    WorldPosition m_AttackTarget{};
    WorldPosition m_BossPosition{0.f, -1150.f};
    bool m_Ranged = false;
    bool m_WasHeld = false;
    bool m_ResetCombat = false;
    float m_ParryWindow = 0.f;
    float m_ParryCooldown = 0.f;
    std::array<float, 4> m_Delivery{};
};
