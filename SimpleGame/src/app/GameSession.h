#pragma once
#include "FieldPopulation.h"

#include "EnemySystem.h"
#include "Chapter.h"
#include <functional>
#include <memory>
#include <unordered_map>
#include "LootSystem.h"
#include "PlayerInventory.h"
#include "PlayerProgression.h"
#include <string>
#include "WorldAtlas.h"

class GameSession
{
  public:
    using ChapterFactory = std::function<std::unique_ptr<Chapter>()>;
    GameSession();
    void RegisterChapter(std::string id, ChapterFactory factory);
    void OpenWorld(LevelMap& map, const std::string& startChapter = {});
    void FollowPlayer(WorldPosition player);
    bool WorldEnabled() const;
    WorldPosition LocalPlayer(WorldPosition world) const;
    WorldPosition WorldOrigin() const;
    bool BlocksMovement(WorldPosition world) const;
    bool InCombat(WorldPosition world) const;
    const WorldAtlas& Atlas() const;
    MonitorSnapshot CaptureMonitoring(const LevelMap& map, WorldPosition player) const;
    std::optional<WorldMarker> TrackedGoal() const;
    WorldPosition Checkpoint() const;
    bool ReturnToCamp(WorldPosition player, WorldPosition& destination);
    const Chapter& StoryChapter() const;
    bool CanSkipChapter1() const;
    bool CanSkipChapter2FieldStory() const;
    bool CanSkipChapter2() const;
    bool CanSkipChapter3() const;
    bool SkipChapter1(WorldPosition& destination);
    bool SkipChapter2FieldStory(WorldPosition& destination);
    bool SkipChapter2(WorldPosition& destination);
    bool SkipChapter3(WorldPosition& destination);
    bool DrawWorldLandmark(Renderer& renderer, const MapLandmark& site, Point p, float time) const;
    void AddWorldActors(SceneGraph& scene, WorldPosition player) const;
    void DrawWorldEnemy(Renderer& renderer, int region, int enemy, Point p) const;
    const LootDrop& WorldDrop(int region, int drop) const;
    bool TravelTo(const ChapterTravel& travel, LevelMap& map, WorldPosition& destination);
    bool ApplyPendingTravel(LevelMap& map, WorldPosition& destination);
    const std::string& ActiveChapterId() const;
    const Chapter* FindChapter(const std::string& id) const;
    void Reset(int profession, unsigned seed, LevelMap& map);
    void Strike(int step, WorldPosition player, bool faceLeft, const LevelMap& map);
    void StrikeArea(WorldPosition center, float radius, int damage);
    void Update(float dt, WorldPosition player, const LevelMap& map, bool interactionHeld = false);
    void Revive();
    WorldPosition ConsumePush();
    bool IsDefeated() const;
    bool Complete() const;
    std::string Objective() const;
    const std::string& Notice() const;
    std::string LandmarkPrompt(const MapLandmark& site) const;
    bool HasLandmarkInteraction() const;
    const std::string& LandmarkInteractionText() const;
    WorldPosition LandmarkInteractionPosition() const;
    LandmarkKind LandmarkInteractionKind() const;
    const PlayerProgression& Progress() const;
    PlayerProgression& Progress();
    const PlayerInventory& Inventory() const;
    const EnemySystem& Enemies() const;
    const LootSystem& Loot() const;
    int Kills() const;
    void Inspect(const MapLandmark& site, const LevelMap& map);
    Chapter& CurrentChapter();
    const Chapter& CurrentChapter() const;
    void ShowSkillNotice(const std::string& text);
    void StrikeProjectile(WorldPosition from,
                          WorldPosition to,
                          float radiusX,
                          float radiusY,
                          int damage,
                          std::unordered_set<std::uint64_t>& hitTargets,
                          const LevelMap& map,
                          float height = LevelTuning::ProjectileHeight,
                          std::optional<WorldPosition> sightOrigin = std::nullopt);

  private:
    void ActivateRegion(std::size_t region);
    void SynchronizeChapterHistory();
    bool StoryReady() const;
    enum class StorySkip
    {
        Chapter1,
        Chapter2Field,
        Chapter2,
        Chapter3
    };
    bool SkipStory(StorySkip target, WorldPosition& destination);

    struct RegionRuntime
    {
        LevelMap map;
        EnemySystem enemies;
        FieldPopulation population;
        LootSystem loot;
    };

    WorldAtlas m_Atlas;
    std::vector<RegionRuntime> m_Regions;
    std::size_t m_Region = 0;
    bool m_OpenWorld = false;
    bool m_StartAtChapter2 = false;
    bool m_StartAtChapter3 = false;
    WorldPosition m_Checkpoint{};
    void ClearMapActors();
    void ShowNotice(const std::string& text);
    void RewardDefeated(const std::vector<WorldPosition>& defeated);
    void RewardDefeated(const std::vector<WorldPosition>& defeated, LootSystem& loot);
    void CollectLoot(const std::vector<LootDrop>& drops);
    PlayerProgression m_Progress;
    PlayerInventory m_Inventory;

    struct ChapterSlot
    {
        ChapterFactory factory;
        std::unique_ptr<Chapter> instance;
    };

    std::unordered_map<std::string, ChapterSlot> m_Chapters;
    Chapter* m_Chapter = nullptr;
    std::string m_ActiveChapterId;
    EnemySystem m_Enemies;
    LootSystem m_Loot;
    int m_Profession = 0;
    int m_Kills = 0;
    float m_Invulnerable = 0.f;
    WorldPosition m_Push{};
    float m_NoticeTime = 0.f;
    std::string m_Notice;
    float m_LandmarkInteractionTime = 0.f;
    std::string m_LandmarkInteractionText;
    WorldPosition m_LandmarkInteractionPosition{};
    LandmarkKind m_LandmarkInteractionKind = LandmarkKind::Sign;
};
