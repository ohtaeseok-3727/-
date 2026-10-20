#pragma once

#include "Renderer.h"

class SceneCamera
{
  public:
    SceneCamera(float x, float y, int width, int height);
    Point Project(float x, float y) const;
    bool IsVisible(Point point, float margin = 0.f) const;
    static float GroundDepthScale();

  private:
    float m_X;
    float m_Y;
    int m_Width;
    int m_Height;
};
