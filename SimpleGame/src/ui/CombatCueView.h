#pragma once
#include "Renderer.h"
#include "EnemySystem.h"

class CombatCueView
{
  public:
    static void DrawEnemy(Renderer& renderer, const FieldEnemy& enemy, Point foot, float height);
    static void DrawArea(Renderer& renderer, Point center, float radius, float progress);
};
