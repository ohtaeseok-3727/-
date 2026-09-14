#pragma once

struct WorldPosition
{
    float x = 0.f;
    float y = 0.f;
};

struct WorldProp
{
    float x;
    float y;
    int kind;
    float size;
    int entity = -1;
};

enum class LootKind
{
    Gold,
    Equipment,
    Health
};

// Initial tuning for the first farming area, not final game balance.
namespace LevelTuning
{
    constexpr int EnemyCount = 10;
    constexpr int EnemyHealth = 45;
    constexpr int EnemyDamage = 8;
    constexpr int KillExperience = 25;
    constexpr float MagnetRadius = 150.f;
    constexpr float PickupRadius = 16.f;
    constexpr float RespawnSeconds = 10.f;
    constexpr float HitRange = 95.f;
}
