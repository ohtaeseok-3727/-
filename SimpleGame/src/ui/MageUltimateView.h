#pragma once
#include "Renderer.h"
class MageUltimate;

class MageUltimateView
{
  public:
    static void DrawFloor(Renderer& renderer, const MageUltimate& skill, Point center);
    static void DrawSky(Renderer& renderer, const MageUltimate& skill, Point center);
    static void DrawCaster(Renderer& renderer, const MageUltimate& skill, Point feet);
};
