#pragma once
#include "Dependencies/glew.h"

class FrameProfiler;

// Analytic, texture-free HDR effects. Owned by Renderer while its GL context is alive.
class ArcaneEffect
{
  public:
    enum class Kind
    {
        Seal,
        Convergence,
        Explosion
    };
    ArcaneEffect() = default;
    ArcaneEffect(const ArcaneEffect&) = delete;
    ArcaneEffect& operator=(const ArcaneEffect&) = delete;
    ~ArcaneEffect();
    bool Initialize();
    bool Draw(Kind kind,
              float x,
              float y,
              float radius,
              float depth,
              float rotation,
              float time,
              float red,
              float green,
              float blue,
              float opacity,
              int width,
              int height,
              bool linearOutput,
              FrameProfiler& profiler);

  private:
    GLuint m_Program = 0, m_Array = 0;
    GLint m_Viewport = -1, m_Rect = -1, m_Params = -1, m_Tint = -1, m_Kind = -1, m_Linear = -1;
    bool m_Attempted = false;
};
