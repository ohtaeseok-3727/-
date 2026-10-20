#pragma once

#include "MapLayout.h"

// Authored city kit; buildings own their footprints, rooms own closed portal-only boundaries.
class Chapter3Architecture
{
  public:
    static void DressCity(MapDefinition& map);
    static void EncloseRoom(MapDefinition& map, MapRectangle bounds);
    static void DressReadingRoom(MapDefinition& map);
};
