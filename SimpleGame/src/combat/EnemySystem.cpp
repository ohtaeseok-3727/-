#include "stdafx.h"
#include "EnemySystem.h"
#include "SceneCamera.h"
#include "MageProjectiles.h"
#include <algorithm>
#include <cmath>

void EnemySystem::Reset(unsigned, const LevelMap&)
{
    Clear();
}

void EnemySystem::Clear()
{
    m_Enemies.clear();
    m_ScriptDefeats.clear();
    m_UpdateStart = 0;
    m_SupportEnabled = true;
}

void EnemySystem::Add(WorldPosition position,
                      std::size_t encounter,
                      EnemyRole role,
                      const EnemyStats& stats)
{
    FieldEnemy enemy;
    enemy.id = m_NextId++;
    enemy.position = position;
    enemy.encounter = encounter;
    enemy.role = role;
    enemy.stats = stats;
    enemy.maxHealth = (std::max)(1, stats.health);
    enemy.stats.minimumHealth = (std::max)(0, (std::min)(enemy.maxHealth, stats.minimumHealth));
    enemy.health = enemy.maxHealth;
    // Reuse dead slots; army pressure cannot grow the backing array forever.
    const auto slot = std::find_if(m_Enemies.begin(),
                                   m_Enemies.end(),
                                   [](const FieldEnemy& item)
                                   {
                                       return item.health <= 0 && !item.stats.preserveCorpse;
                                   });
    if (slot != m_Enemies.end())
    {
        *slot = enemy;
    }
    else
    {
        m_Enemies.push_back(enemy);
    }
}

bool EnemySystem::Alive(std::size_t encounter) const
{
    return std::any_of(m_Enemies.begin(),
                       m_Enemies.end(),
                       [encounter](const FieldEnemy& enemy)
                       {
                           return enemy.encounter == encounter && enemy.health > 0;
                       });
}

void EnemySystem::RemoveEncounter(std::size_t encounter)
{
    for (auto& enemy : m_Enemies)
    {
        if (enemy.encounter == encounter)
        {
            enemy.health = 0;
            m_StateMachine.Interrupt(enemy);
        }
    }
}

FieldEnemy* EnemySystem::Boss()
{
    const auto boss = std::find_if(m_Enemies.begin(),
                                   m_Enemies.end(),
                                   [](const FieldEnemy& enemy)
                                   {
                                       return enemy.role == EnemyRole::Boss;
                                   });
    return boss == m_Enemies.end() ? nullptr : &*boss;
}

void EnemySystem::SetSupportEnabled(bool enabled)
{
    m_SupportEnabled = enabled;
}

std::vector<WorldPosition> EnemySystem::ConsumeScriptDefeats()
{
    std::vector<WorldPosition> defeated;
    defeated.swap(m_ScriptDefeats);
    return defeated;
}

bool EnemySystem::DamageEnemy(std::uint64_t id, int damage)
{
    for (auto& enemy : m_Enemies)
    {
        if (enemy.id == id && enemy.health > 0)
        {
            enemy.health =
                (std::max)(enemy.stats.minimumHealth, enemy.health - (std::max)(0, damage));
            if (enemy.health == 0)
            {
                m_ScriptDefeats.push_back(enemy.position);
            }
            enemy.hitFlash = .18f;
            if (!enemy.stats.scriptedAI)
            {
                m_StateMachine.Interrupt(enemy);
                enemy.cooldown = .8f;
            }
            return true;
        }
    }
    return false;
}

int EnemySystem::Update(float dt, WorldPosition player, const LevelMap& map)
{
    if (dt <= 0.f || m_Enemies.empty())
    {
        return 0;
    }
    int incomingDamage = 0;
    int searchBudget = 2;
    // Rotate the first updated enemy so a sustained search backlog is served fairly.
    for (std::size_t offset = 0; offset < m_Enemies.size(); ++offset)
    {
        auto& enemy = m_Enemies[(m_UpdateStart + offset) % m_Enemies.size()];
        incomingDamage =
            (std::max)(incomingDamage,
                       m_StateMachine.Update(
                           enemy, m_Enemies, dt, player, map, m_SupportEnabled, searchBudget));
    }
    m_UpdateStart = (m_UpdateStart + 1) % m_Enemies.size();
    return incomingDamage;
}

std::vector<WorldPosition> EnemySystem::Strike(WorldPosition player,
                                               bool faceLeft,
                                               int damage,
                                               const LevelMap& map)
{
    std::vector<WorldPosition> defeated;
    for (auto& enemy : m_Enemies)
    {
        const float dx = (enemy.position.x - player.x) * (faceLeft ? -1.f : 1.f);
        const float dy = enemy.position.y - player.y;
        if (enemy.health <= 0 || dx < -18.f ||
            dx * dx + dy * dy * 2 > LevelTuning::HitRange * LevelTuning::HitRange ||
            !map.ClearPath(player, enemy.position))
        {
            continue;
        }
        enemy.health = (std::max)(enemy.stats.minimumHealth, enemy.health - damage);
        enemy.hitFlash = .18f;
        if (!enemy.stats.scriptedAI)
        {
            m_StateMachine.Interrupt(enemy);
            enemy.cooldown = .8f;
        }
        if (enemy.health == 0)
        {
            defeated.push_back(enemy.position);
        }
    }
    return defeated;
}

const std::vector<FieldEnemy>& EnemySystem::Enemies() const
{
    return m_Enemies;
}

std::vector<WorldPosition> EnemySystem::StrikeProjectile(
    WorldPosition from,
    WorldPosition to,
    float radiusX,
    float radiusY,
    int damage,
    std::unordered_set<std::uint64_t>& hitTargets,
    const LevelMap& map)
{
    std::vector<WorldPosition> defeated;
    for (auto& enemy : m_Enemies)
    {
        const float x = (std::max)((std::min)(from.x, to.x),
                                   (std::min)((std::max)(from.x, to.x), enemy.position.x));
        // Compare projected projectile centers with the enemy's torso, not its feet.
        const float dy = (enemy.position.y - to.y) * SceneCamera::GroundDepthScale() +
                         MageProjectiles::Height - 35.f;
        if (enemy.health <= 0 || hitTargets.count(enemy.id) ||
            std::abs(enemy.position.x - x) > radiusX + 16.f || std::abs(dy) > radiusY + 25.f ||
            !map.ClearPath({x, to.y}, enemy.position))
        {
            continue;
        }
        hitTargets.insert(enemy.id);
        enemy.health = (std::max)(enemy.stats.minimumHealth, enemy.health - damage);
        enemy.hitFlash = .18f;
        if (!enemy.stats.scriptedAI)
        {
            m_StateMachine.Interrupt(enemy);
            enemy.cooldown = .8f;
        }
        if (enemy.health == 0)
        {
            defeated.push_back(enemy.position);
        }
    }
    return defeated;
}
