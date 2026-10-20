#pragma once

#include "GameSession.h"
#include "Renderer.h"

class ExpeditionView
{
  public:
    void DrawGuide(Renderer& renderer,
                   const GameSession& session,
                   const LevelMap& map,
                   WorldPosition player,
                   int width) const;
    void DrawMap(Renderer& renderer,
                 const GameSession& session,
                 const LevelMap& map,
                 WorldPosition player,
                 int width,
                 int height) const;
    void DrawJournal(Renderer& renderer, const GameSession& session, int width, int height) const;
    void DrawDefeat(Renderer& renderer, int width, int height) const;
};
