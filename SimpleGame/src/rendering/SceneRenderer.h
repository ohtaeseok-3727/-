#pragma once

#include "SceneGraph.h"

class BasicAttack;
class MageProjectiles;
class MageUltimate;
class KnightSkills;
class LevelMap;
class GameSession;
class GameView;
class MapView;

struct SceneFrame
{
    const LevelMap& map;
    const GameSession& level;
    const BasicAttack& attack;
    const MageProjectiles& mageProjectiles;
    const MageUltimate& mageUltimate;
    const KnightSkills& knightSkills;
    GameView& levelView;
    MapView& mapView;
    float time;
    float walk;
    int profession;
    bool moving;
    bool faceLeft;
    bool started;
    Point playerScreen;
    int width;
    int height;
};

// Converts ordered scene items into the low-level Renderer's existing batches.
class SceneRenderer
{
  public:
    void Draw(Renderer& renderer, const SceneGraph& graph, const SceneFrame& frame) const;

  private:
    void DrawActor(Renderer& renderer, const SceneItem& item, const SceneFrame& frame) const;
};
