#include "stdafx.h"
#include "SceneRenderer.h"
#include "BasicAttack.h"
#include "MageProjectiles.h"
#include "MageProjectileView.h"
#include "LevelMap.h"
#include "GameSession.h"
#include "GameView.h"
#include "MapView.h"
#include <cmath>

namespace
{
    const float objectHeightScale = std::cos(60.f * 3.14159265358979323846f / 180.f);
}

void SceneRenderer::Draw(Renderer& r, const SceneGraph& graph, const SceneFrame& frame) const
{
    const float depthScale = SceneCamera::GroundDepthScale();
    for (const auto& item : graph.StaticVisible())
    {
        const Point p = item.screen;
        const float size = item.actor->Description().size;
        if (p.x > -150 && p.x < frame.width + 150 && p.y > -80 && p.y < frame.height + 80)
        {
            r.Ellipse(
                p.x + 8, p.y, 30 * size, 19 * depthScale * size, Color(.025f, .035f, .035f, .35f));
        }
    }
    if (frame.started)
    {
        const Point p = frame.playerScreen;
        r.Ellipse(p.x, p.y, 10, 7 * depthScale, Color(.015f, .025f, .03f, .6f));
        r.Ellipse(p.x, p.y, 11, 7 * depthScale, Color(.7f, .62f, .37f, .13f));
    }
    for (const auto& item : graph.RenderQueue())
    {
        DrawActor(r, item, frame);
    }
}

void SceneRenderer::DrawActor(Renderer& r, const SceneItem& item, const SceneFrame& frame) const
{
    const auto& o = item.actor->Description();
    Point p = item.screen;
    float x = p.x, y = p.y, s = o.size;
    const bool cacheable = o.kind >= 0 && o.kind <= 2;
    if (cacheable)
    {
        if (r.CachedGeometry(o.kind, x, y, s))
        {
            return;
        }
        r.BeginGeometry();
        p = {0.f, 0.f};
        x = y = 0.f;
        s = 1.f;
    }
    // Solid props share the ground projection; characters/trees remain upright billboards.
    const auto point = [&](float dx, float depth, float height)
    {
        Point q = {p.x + dx * s, p.y + depth * s * SceneCamera::GroundDepthScale()};
        q.y -= height * objectHeightScale * s;
        return q;
    };
    if (o.kind == 9)
    {
        const auto& site = frame.map.Layout().Landmarks()[o.entity];
        if (!frame.level.CurrentChapter().DrawLandmark(r, site, p, frame.time))
        {
            frame.mapView.DrawLandmark(r, site, p, frame.time);
        }
    }
    else if (o.kind == 10)
    {
        frame.level.CurrentChapter().DrawActor(r, o.entity, p, frame.time);
    }
    else if (o.kind == 6)
    {
        frame.level.CurrentChapter().DrawEnemy(r, frame.level.Enemies().Enemies()[o.entity], p);
    }
    else if (o.kind == 7)
    {
        frame.levelView.DrawDrop(r, frame.level.Loot().Drops()[o.entity], p);
    }
    else if (o.kind == 11)
    {
        MageProjectileView::Draw(r, frame.mageProjectiles.Projectiles()[o.entity], p);
    }
    else if (o.kind == 5)
    {
        if (frame.mageProjectiles.Casting() &&
            r.CombatVisual("mage-2",
                           frame.mageProjectiles.CastMilliseconds(),
                           x,
                           y,
                           frame.mageProjectiles.CastFaceLeft(),
                           MageProjectiles::CharacterScale,
                           false))
        {
            return;
        }
        if (frame.attack.IsAttacking() && r.CharacterAttack(frame.profession,
                                                            frame.attack.ClipId(),
                                                            frame.attack.ElapsedMilliseconds(),
                                                            x,
                                                            y,
                                                            frame.faceLeft))
        {
            return;
        }

        const int walkFrame = frame.moving ? static_cast<int>(frame.walk) : 0;
        r.Character(frame.profession, x, y, frame.faceLeft, 1, -1, walkFrame);
    }
    else if (o.kind == 0)
    {
        r.Quad({x - 7 * s, y},
               {x + 7 * s, y},
               {x + 3 * s, y - 104 * s},
               {x - 4 * s, y - 108 * s},
               Color(.19f, .18f, .16f));
        r.Quad({x, y - 52 * s},
               {x - 38 * s, y - 83 * s},
               {x - 42 * s, y - 106 * s},
               {x - 30 * s, y - 83 * s},
               Color(.22f, .21f, .18f));
        r.Quad({x, y - 73 * s},
               {x + 34 * s, y - 101 * s},
               {x + 39 * s, y - 129 * s},
               {x + 27 * s, y - 102 * s},
               Color(.23f, .22f, .18f));
        r.Triangle({x - 4 * s, y - 89 * s},
                   {x + 3 * s, y - 126 * s},
                   {x + 7 * s, y - 74 * s},
                   Color(.26f, .25f, .21f));
    }
    else if (o.kind == 1)
    {
        r.Quad(point(-38, 14, 0),
               point(25, 14, 0),
               point(25, 14, 128),
               point(-38, 14, 104),
               Color(.36f, .38f, .35f));
        r.Quad(point(25, 14, 0),
               point(40, -28, 0),
               point(40, -28, 128),
               point(25, 14, 128),
               Color(.23f, .27f, .27f));
        r.Quad(point(-38, 14, 104),
               point(25, 14, 128),
               point(40, -28, 128),
               point(-22, -28, 104),
               Color(.48f, .49f, .42f));
        for (int i = 1; i < 4; ++i)
        {
            Point seam = point(-37, 14, float(i * 26));
            r.Rect(seam.x, seam.y, 61 * s, 2 * s, Color(.23f, .26f, .25f));
        }
        Point crack = point(-7, 14, 96);
        r.Rect(crack.x, crack.y, 3 * s, 19 * s, Color(.19f, .23f, .22f));
        crack = point(10, 14, 42);
        r.Rect(crack.x, crack.y, 3 * s, 19 * s, Color(.19f, .23f, .22f));
        r.Triangle(
            point(-32, 18, 0), point(-49, 28, 0), point(-18, 30, 0), Color(.38f, .39f, .34f));
    }
    else if (o.kind == 2)
    {
        r.Quad(point(-20, 10, 0),
               point(-13, -10, 28),
               point(11, -14, 32),
               point(22, 7, 0),
               Color(.30f, .34f, .32f));
        r.Triangle(
            point(-13, -10, 28), point(11, -14, 32), point(2, 5, 24), Color(.43f, .46f, .40f));
    }
    else if (o.kind == 3)
    {
        r.Rect(x - 2, y - 96, 4, 96, Color(.39f, .33f, .24f));
        float sway = std::sin(frame.time * 2) * 4;
        r.Quad({x + 2, y - 94},
               {x + 43, y - 90 + sway},
               {x + 36, y - 51 + sway},
               {x + 2, y - 60},
               Color(.43f, .19f, .17f));
        r.Rect(x + 14, y - 81, 3, 19, Color(.70f, .57f, .36f));
    }
    else
    {
        r.Rect(x - 14, y - 5, 28, 5, Color(.22f, .18f, .13f));
        float flicker = std::sin(frame.time * 9) * 4;
        // Draw only the emitter. The post-process bright pass supplies all light bloom.
        // Gentle intensity variation changes the bloom without drawing a flat halo.
        float intensity = 1.f + .06f * std::sin(frame.time * 7.f);
        r.Triangle({x - 10, y - 5},
                   {x + 11, y - 5},
                   {x + flicker, y - 37},
                   Color(1.65f * intensity, .65f * intensity, .16f * intensity));
        r.Triangle({x - 5, y - 6},
                   {x + 5, y - 6},
                   {x - flicker, y - 25},
                   Color(2.4f * intensity, 1.25f * intensity, .38f * intensity));
    }
    if (cacheable)
    {
        r.EndGeometry(o.kind, item.screen.x, item.screen.y, o.size);
    }
}
