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

    bool IsOpen() const
    {
        return m_OverviewOpen;
    }

  private:
    struct GroundTile
    {
        int x = 0;
        int y = 0;
        bool valid = false;
        GeometryCache::Mesh mesh;
    };

    const GeometryCache::Mesh& GroundGeometry(const LevelMap& map, int x, int y) const;
    mutable std::vector<GroundTile> m_GroundTiles;
    mutable const LevelMap* m_GroundMap = nullptr;
    mutable std::uint64_t m_GroundRevision = 0;
    mutable std::uint32_t m_GroundSeed = 0;
    bool m_OverviewOpen = false;
};
