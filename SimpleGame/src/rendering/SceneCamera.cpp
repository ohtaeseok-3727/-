#include "stdafx.h"
#include "SceneCamera.h"
#include <cmath>

SceneCamera::SceneCamera(float x, float y, int width, int height)
    : m_X(x), m_Y(y), m_Width(width), m_Height(height)
{
}

float SceneCamera::GroundDepthScale()
{
    static const float scale = std::sin(60.f * 3.14159265358979323846f / 180.f);
    return scale;
}

Point SceneCamera::Project(float x, float y) const
{
    return {m_Width * .5f + x - m_X, m_Height * .52f + (y - m_Y) * GroundDepthScale()};
}

bool SceneCamera::IsVisible(Point point) const
{
    // Preserve the existing conservative bounds for tall props and attack effects.
    return point.x >= -180.f && point.x <= m_Width + 180.f && point.y >= -80.f &&
           point.y <= m_Height + 230.f;
}
