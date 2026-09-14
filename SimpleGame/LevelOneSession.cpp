#include "stdafx.h"
#include "LevelOneSession.h"
#include <algorithm>

void LevelOneSession::Reset(int profession, unsigned seed, const LevelMap& map)
{
    m_Profession = profession;
    m_Kills = 0;
    m_CompletionAnnounced = false;
    m_Invulnerable = 0.f;
    m_Progress.Reset(profession);
    m_Inventory.Reset();
    m_Enemies.Reset(seed, map);
    m_Loot.Reset(seed);
    ShowNotice(
        "Read the village board [E], then follow the NORTH road. Press M for the route map.");
}

void LevelOneSession::Strike(int step, WorldPosition player, bool faceLeft, const LevelMap& map)
{
    const int damage =
        (m_Progress.Attack() + m_Inventory.AttackBonus()) * (100 + (step - 1) * 25) / 100;
    for (const auto& position : m_Enemies.Strike(player, faceLeft, damage, map))
    {
        ++m_Kills;
        const int levels = m_Progress.AddExperience(LevelTuning::KillExperience);
        m_Loot.Drop(position, m_Kills, m_Progress.Level());
        if (levels > 0)
        {
            ShowNotice("LEVEL UP! Level " + std::to_string(m_Progress.Level()) +
                       " | HP / MP / primary stat increased");
        }
        else
        {
            ShowNotice("Creature defeated: +25 EXP. Move closer to attract its drops.");
        }
    }
}

void LevelOneSession::Update(float dt, WorldPosition player, const LevelMap& map)
{
    m_NoticeTime = (std::max)(0.f, m_NoticeTime - dt);
    if (m_NoticeTime == 0.f)
    {
        m_Notice.clear();
    }
    m_Invulnerable = (std::max)(0.f, m_Invulnerable - dt);
    const int damage = m_Enemies.Update(dt, player, map);
    if (damage > 0 && m_Invulnerable <= 0.f)
    {
        m_Progress.Damage(damage);
        m_Invulnerable = .8f;
        ShowNotice("Hit! Step away when a creature turns orange. Red drops restore HP.");
    }
    // Defeat is resolved before pickups so a lethal hit cannot be silently undone.
    if (IsDefeated())
    {
        return;
    }
    for (const auto& drop : m_Loot.Update(dt, player))
    {
        switch (drop.kind)
        {
        case LootKind::Gold:
            m_Inventory.AddGold(drop.value);
            break;
        case LootKind::Equipment:
            m_Inventory.AddEquipment(m_Profession, drop.value);
            ShowNotice("Equipment collected! Strongest gear equipped. Press I to inspect.");
            break;
        case LootKind::Health:
            m_Progress.Heal(drop.value);
            break;
        }
    }
    if (!m_CompletionAnnounced && Complete())
    {
        m_CompletionAnnounced = true;
        ShowNotice(
            "FARMING BASICS COMPLETE. Inspect the abandoned farm, then return to the veteran.");
    }
}

void LevelOneSession::Revive()
{
    m_Progress.Heal(m_Progress.MaxHealth());
    m_Invulnerable = 3.f;
    ShowNotice("Returned to the village. Your level, money and equipment are safe.");
}

bool LevelOneSession::IsDefeated() const
{
    return m_Progress.Health() <= 0;
}

bool LevelOneSession::Complete() const
{
    return m_Progress.Level() >= 3 && m_Inventory.Gold() > 0 && !m_Inventory.Items().empty();
}

const char* LevelOneSession::Objective() const
{
    if (m_Kills == 0)
    {
        return "Read the board [E], then take the NORTH road to the first farming contract. [M] Map";
    }
    if (m_Progress.Level() < 2)
    {
        return "2. Defeat creatures to gain EXP and reach character level 2.";
    }
    if (m_Inventory.Gold() == 0 || m_Inventory.Items().empty())
    {
        return "3. Approach gold / blue drops: collect money and equipment automatically.";
    }
    if (m_Progress.Level() < 3)
    {
        return "4. Reach character level 3. Red drops restore HP; orange enemies are attacking.";
    }
    return "Follow the farm tracks; return to town; investigate river / woods / battlefield. [M] Map";
}

void LevelOneSession::Inspect(const TutorialLandmark& site)
{
    ShowNotice(site.description);
    m_NoticeTime = 9.f;
}

void LevelOneSession::ShowNotice(const std::string& text)
{
    m_Notice = text;
    m_NoticeTime = 4.f;
}

const std::string& LevelOneSession::Notice() const
{
    return m_Notice;
}

const PlayerProgression& LevelOneSession::Progress() const
{
    return m_Progress;
}

const PlayerInventory& LevelOneSession::Inventory() const
{
    return m_Inventory;
}

const EnemySystem& LevelOneSession::Enemies() const
{
    return m_Enemies;
}

const LootSystem& LevelOneSession::Loot() const
{
    return m_Loot;
}

int LevelOneSession::Kills() const
{
    return m_Kills;
}
