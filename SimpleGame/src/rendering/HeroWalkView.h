#pragma once
#include "Renderer.h"

// Owns the accepted hero movement atlas; attack atlases remain with chapter renderers.
class HeroWalkView
{
  public:
    HeroWalkView() = default;
    ~HeroWalkView();
    HeroWalkView(const HeroWalkView&) = delete;
    HeroWalkView& operator=(const HeroWalkView&) = delete;
    bool Draw(Renderer& renderer,
              Point foot,
              float height,
              bool flip,
              float time = 0.f,
              bool moving = false);

  private:
    bool Load();
    bool m_Attempted = false;
    GLuint m_Texture = 0;
};
