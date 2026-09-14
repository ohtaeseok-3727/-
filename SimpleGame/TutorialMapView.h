#pragma once

#include "LevelMap.h"
#include "Renderer.h"

class TutorialMapView
{
  public:
    void DrawTile(Renderer& renderer, const LevelMap& map, int x, int y, Point screen) const;
    void DrawLandmark(Renderer& renderer, const TutorialLandmark& landmark, Point screen) const;
    void DrawOverview(
        Renderer& renderer, const LevelMap& map, WorldPosition player, int width, int height) const;
    void ToggleOverview();

  private:
    bool m_OverviewOpen = false;
};
