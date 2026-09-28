#pragma once

#include "LevelTypes.h"

// Render-facing snapshot. Gameplay state remains owned by the session systems.
class Actor
{
  public:
    explicit Actor(const WorldProp& description);
    const WorldProp& Description() const;
    float Depth() const;

  private:
    WorldProp m_Description;
};
