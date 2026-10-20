#pragma once

#include "MapLayout.h"
#include "Renderer.h"

class BuildingView
{
  public:
    static void Draw(Renderer& renderer, const MapBuilding& building, Point front);
};
