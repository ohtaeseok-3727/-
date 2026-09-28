#pragma once

#include "LevelTypes.h"
#include <cstdint>
#include <vector>

enum class EnemyState
{
    Idle,
    Chase,
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
};
