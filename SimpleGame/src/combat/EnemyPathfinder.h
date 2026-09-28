#pragma once

#include "LevelMap.h"
#include <vector>

class EnemyPathfinder
{
  public:
    bool FindPath(WorldPosition start,
                  WorldPosition goal,
                  const LevelMap& map,
                  std::vector<WorldPosition>& path) const;
};
