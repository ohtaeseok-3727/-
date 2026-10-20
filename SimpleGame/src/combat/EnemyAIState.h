#pragma once

#include "LevelTypes.h"
#include <cstdint>
#include <vector>

enum class EnemyState
{
    Idle,
    Chase,
    Patrol,
    Return,
    Windup,
    Recovery,
    Heal,
    Disabled,
    Dead
};

struct EnemyAIState
{
    EnemyState state = EnemyState::Idle;
    std::vector<WorldPosition> path;
    std::size_t waypoint = 0;
    float repathTimer = 0.f;
    std::uint64_t mapRevision = 0;
    WorldPosition home{};
    WorldPosition patrolTarget{};
    WorldPosition lastSeen{};
    float perceptionTimer = 0.f;
    float lostSightTime = 0.f;
    float patrolWait = 0.f;
    unsigned patrolStep = 0;
    bool alerted = false;
    bool returning = false;
};
