#pragma once

#include "EnemySystem.h"
#include "LootSystem.h"
#include "PlayerInventory.h"
#include "PlayerProgression.h"
#include <string>

class LevelOneSession
{
  public:
    void Reset(int profession, unsigned seed, const LevelMap& map);
    void Strike(int step, WorldPosition player, bool faceLeft, const LevelMap& map);
    void Update(float dt, WorldPosition player, const LevelMap& map);
    void Revive();
    bool IsDefeated() const;
    bool Complete() const;
    const char* Objective() const;
    const std::string& Notice() const;
    const PlayerProgression& Progress() const;
    const PlayerInventory& Inventory() const;
    const EnemySystem& Enemies() const;
    const LootSystem& Loot() const;
    int Kills() const;

  private:
    void ShowNotice(const std::string& text);
    PlayerProgression m_Progress;
    PlayerInventory m_Inventory;
    EnemySystem m_Enemies;
    LootSystem m_Loot;
    int m_Profession = 0;
    int m_Kills = 0;
    bool m_CompletionAnnounced = false;
    float m_Invulnerable = 0.f;
    float m_NoticeTime = 0.f;
    std::string m_Notice;
};
