#pragma once
#include "Renderer.h"
struct MageProjectile;

class MageProjectileView
{
  public:
    static void Draw(Renderer& renderer, const MageProjectile& shot, Point ground);
};
