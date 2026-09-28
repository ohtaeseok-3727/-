#include "stdafx.h"
#include "MageProjectileView.h"
#include "MageProjectiles.h"
#include <algorithm>

void MageProjectileView::Draw(Renderer& r, const MageProjectile& shot, Point ground)
{
    if (!shot.Visible())
    {
        return;
    }
    const float sign = shot.faceLeft ? -1.f : 1.f;
    const float scale =
        shot.kind == MageProjectileKind::Sword ? MageProjectiles::PlayerScale / .36f : 1.f;
    const Point center{ground.x, ground.y - MageProjectiles::Height};
    const auto p = [&](float x, float y)
    {
        return Point{center.x + sign * x * scale, center.y + y * scale};
    };
    if (shot.kind == MageProjectileKind::Wind)
    {
        const float a[] = {1.2f, 1.375f, 1.5f};
        const float c[] = {0.f, -.4375f, 0.f};
        const float d[] = {1.f, .9f, .5f};
        const float half = 660.f * MageProjectiles::PlayerScale * .5f;
        const auto corner = [&](float x, float y)
        {
            return p(a[shot.variant] * x + c[shot.variant] * y, d[shot.variant] * y);
        };
        r.ImageQuad(L"Skills/wind-blade-01.png",
                    corner(-half, -half),
                    corner(half, -half),
                    corner(half, half),
                    corner(-half, half));
        return;
    }
    const float alpha = (std::min)(1.f, shot.age / .250f);
    if (shot.age >= shot.delay)
    {
        r.ImageQuad(
            L"Skills/dash-trail.png", p(-160, -90), p(20, -90), p(20, 90), p(-160, 90), alpha);
    }
    const auto color = [alpha](int red, int green, int blue)
    {
        return Color(red / 255.f, green / 255.f, blue / 255.f, alpha);
    };
    r.Quad(p(-19, -8), p(25, -8), p(25, 8), p(-19, 8), color(23, 62, 75));
    r.Triangle(p(25, -8), p(43, 0), p(25, 8), color(23, 62, 75));
    r.Quad(p(-17, -5), p(25, -5), p(39, 0), p(-17, 0), color(70, 217, 219));
    r.Quad(p(-17, 0), p(39, 0), p(25, 5), p(-17, 5), color(32, 133, 152));
    r.Quad(p(-17, -1), p(30, -1), p(30, 1), p(-17, 1), color(185, 243, 232));
    r.Quad(p(-22, -12), p(-17, -12), p(-17, 12), p(-22, 12), color(36, 157, 173));
    r.Quad(p(-36, -3), p(-22, -3), p(-22, 3), p(-36, 3), color(36, 157, 173));
    r.Quad(p(-39, -5), p(-34, -5), p(-34, 5), p(-39, 5), color(70, 217, 219));
}
