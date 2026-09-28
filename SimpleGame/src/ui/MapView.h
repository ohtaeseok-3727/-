#pragma once

#include "LevelMap.h"
#include "Renderer.h"

class MapView
{
  public:
    void DrawTile(Renderer& renderer, const LevelMap& map, int x, int y, Point screen) const;
    void DrawLandmark(Renderer& renderer,
                      const MapLandmark& landmark,
                      Point screen,
                      float animationTime) const;
    float BubbleAnchorHeight(LandmarkKind kind) const;
    void DrawOverview(
        Renderer& renderer, const LevelMap& map, WorldPosition player, int width, int height) const;
    void ToggleOverview();
    void CloseOverview();

  private:
    bool m_OverviewOpen = false;
};
