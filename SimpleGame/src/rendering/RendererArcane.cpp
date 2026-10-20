#include "stdafx.h"
#include "Renderer.h"

bool Renderer::DrawArcane(ArcaneEffect::Kind kind,
                          Point center,
                          float radius,
                          float depth,
                          float rotation,
                          float age,
                          Color color)
{
    if (!m_Initialized)
        return false;
    Flush();
    return m_Arcane.Draw(kind,
                         center.x,
                         center.y,
                         radius,
                         depth,
                         rotation,
                         age,
                         color.r,
                         color.g,
                         color.b,
                         color.a,
                         m_Width,
                         m_Height,
                         m_InLinearScene,
                         m_Profiler);
}
