#pragma once

#include "EnemySystem.h"
#include "Chapter.h"
#include <functional>
#include <memory>
#include <unordered_map>
#include "LootSystem.h"
#include "PlayerInventory.h"
#include "PlayerProgression.h"
#include <string>

class GameSession
{
  public:
    using ChapterFactory = std::function<std::unique_ptr<Chapter>()>;
    GameSession();
    void RegisterChapter(std::string id, ChapterFactory factory);
    bool TravelTo(const ChapterTravel& travel, LevelMap& map, WorldPosition& destination);
    bool ApplyPendingTravel(LevelMap& map, WorldPosition& destination);
    const std::string& ActiveChapterId() const;
    const Chapter* FindChapter(const std::string& id) const;
    void Reset(int profession, unsigned seed, LevelMap& map);
    void Strike(int step, WorldPosition player, bool faceLeft, const LevelMap& map);
    void Update(float dt, WorldPosition player, const LevelMap& map, bool interactionHeld = false);
    void Revive();
    WorldPosition ConsumePush();
    bool IsDefeated() const;
    bool Complete() const;
    std::string Objective() const;
    const std::string& Notice() const;
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
                          const LevelMap& map);

  private:
    void ClearMapActors();
    void ShowNotice(const std::string& text);
    void RewardDefeated(const std::vector<WorldPosition>& defeated);
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
