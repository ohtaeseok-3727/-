#include "stdafx.h"
#include "MageUltimateView.h"
#include "MageUltimateSwordView.h"
#include "MageUltimate.h"
#include "SceneCamera.h"
#include <algorithm>
#include <cmath>

namespace
{
    constexpr float Pi = 3.14159265f;
    constexpr float SkyHeight = 230.f;

    float Ease(float value)
    {
        value = std::clamp(value, 0.f, 1.f);
        return value * value * (3.f - 2.f * value);
    }

    void Line(Renderer& r, Point a, Point b, float width, Color color)
    {
        const float length = std::hypot(b.x - a.x, b.y - a.y);
        if (length < .001f)
            return;
        const Point n{-(b.y - a.y) * width / length, (b.x - a.x) * width / length};
        r.Quad({a.x + n.x, a.y + n.y},
               {b.x + n.x, b.y + n.y},
               {b.x - n.x, b.y - n.y},
               {a.x - n.x, a.y - n.y},
               color);
    }

    Point Polar(Point center, float radius, float angle, float depth)
    {
        return {center.x + std::cos(angle) * radius, center.y + std::sin(angle) * radius * depth};
    }

    void Ring(Renderer& r, Point center, float radius, float depth, float width, Color color)
    {
        if (radius <= width || color.a <= 0.f)
            return;
        for (int i = 0; i < 96; ++i)
        {
            const float a = 2.f * Pi * i / 96.f, b = 2.f * Pi * (i + 1) / 96.f;
            r.Quad(Polar(center, radius - width, a, depth),
                   Polar(center, radius - width, b, depth),
                   Polar(center, radius + width, b, depth),
                   Polar(center, radius + width, a, depth),
                   color);
        }
    }

    void Seal(Renderer& r, Point center, float radius, float depth, float angle, Color ink)
    {
        if (radius < 1.f || ink.a <= 0.f)
            return;
        if (r.DrawArcane(ArcaneEffect::Kind::Seal, center, radius, depth, angle, 0.f, ink))
            return;
        r.Ellipse(
            center.x, center.y, radius, radius * depth, Color(ink.r, ink.g, ink.b, ink.a * .035f));
        Ring(r, center, radius, depth, 6.f, Color(ink.r, ink.g, ink.b, ink.a * .10f));
        Ring(r, center, radius, depth, 1.6f, ink);
        Ring(r, center, radius * .92f, depth, .8f, ink);
        Ring(r, center, radius * .76f, depth, 1.f, ink);
        Ring(r, center, radius * .39f, depth, 1.2f, ink);
        for (int i = 0; i < 48; ++i)
        {
            const float a = angle + i * 2.f * Pi / 48.f;
            Line(r,
                 Polar(center, radius * .96f, a, depth),
                 Polar(center, radius * (i % 4 ? .99f : 1.04f), a, depth),
                 .7f,
                 ink);
        }
        // Geometric rune band and two interwoven triangles, rotating in opposition.
        for (int i = 0; i < 18; ++i)
        {
            const float a = -angle + i * 2.f * Pi / 18.f;
            const Point p = Polar(center, radius * .83f, a, depth);
            Line(r, Polar(p, 9, a, depth), Polar(p, 9, a + Pi, depth), .9f, ink);
            Line(r, Polar(p, 9, a + Pi, depth), Polar(p, 8, a + Pi * .4f, depth), .9f, ink);
            Line(r, Polar(p, 8, a + Pi * .4f, depth), Polar(p, 8, a - Pi * .4f, depth), .9f, ink);
        }
        for (int i = 0; i < 6; ++i)
            Line(r,
                 Polar(center, radius * .72f, angle + i * Pi / 3.f, depth),
                 Polar(center, radius * .72f, angle + (i + 2) * Pi / 3.f, depth),
                 1.f,
                 ink);
        Ring(r, center, radius * .12f, depth, 2.f, Color(.87f, .97f, 1.f, ink.a));
    }

    float Merge(float time)
    {
        return Ease((time - static_cast<float>(MageUltimate::MergeStart)) / .55f);
    }
}

void MageUltimateView::DrawFloor(Renderer& r, const MageUltimate& skill, Point center)
{
    if (!skill.Active())
        return;
    const float t = skill.Time(), grow = Ease(t / static_cast<float>(MageUltimate::SummonDuration));
    const float merge = Merge(t), depth = SceneCamera::GroundDepthScale();
    const float fade = 1.f - Ease((t - static_cast<float>(MageUltimate::ExplosionTime)) / .55f);
    // This boundary is the actual radius-300 ground projection, even while the seals merge.
    Ring(r, center, MageUltimate::Radius * grow, depth, 1.5f, Color(.36f, .65f, 1.f, .24f * fade));
    if (t < MageUltimate::ExplosionTime)
        Seal(r,
             {center.x, center.y - SkyHeight * .5f * merge},
             MageUltimate::Radius * grow * (1.f - .78f * merge),
             depth + (.34f - depth) * merge,
             t * .27f,
             Color(.65f, .45f, 1.f, grow * .8f));
}

void MageUltimateView::DrawSky(Renderer& r, const MageUltimate& skill, Point center)
{
    if (!skill.Active())
        return;
    const float t = skill.Time(), grow = Ease(t / static_cast<float>(MageUltimate::SummonDuration));
    const float merge = Merge(t);
    const Point core{center.x, center.y - SkyHeight * .5f};
    bool gpuEnergy = false;
    if (t < MageUltimate::ExplosionTime)
        gpuEnergy = r.DrawArcane(
            ArcaneEffect::Kind::Convergence, core, 300.f, 1.f, 0.f, t, Color(1.f, 1.f, 1.f, grow));
    else
    {
        const float age = t - static_cast<float>(MageUltimate::ExplosionTime);
        r.ArcaneShockwave() = {
            core.x, core.y, age, 7.f * Ease(age / .025f) * (1.f - Ease(age / .55f))};
        gpuEnergy = r.DrawArcane(ArcaneEffect::Kind::Explosion,
                                 core,
                                 300.f,
                                 1.f,
                                 0.f,
                                 t - static_cast<float>(MageUltimate::ExplosionTime),
                                 Color(1.f, 1.f, 1.f));
    }
    if (t < MageUltimate::ExplosionTime)
    {
        // Tilt the overhead seal toward the viewer so its runes remain visible above the scene.
        Seal(r,
             {center.x, center.y - SkyHeight + SkyHeight * .5f * merge},
             MageUltimate::Radius * grow * (1.f - .78f * merge),
             .34f,
             -t * .34f,
             Color(.32f, .80f, 1.f, grow * .9f));
    }
    for (int pulse = 0; pulse < MageUltimate::PulseCount; ++pulse)
    {
        const float impact = static_cast<float>(MageUltimate::PulseTime(pulse));
        const float start =
            (std::max)(static_cast<float>(MageUltimate::SummonDuration), impact - .24f);
        if (t < start || t >= impact + .08f)
            continue;
        const float progress = std::clamp((t - start) / (impact - start), 0.f, 1.f);
        const float travel = progress * progress;
        const float alpha = 1.f - Ease((t - impact) / .08f);
        for (int sword = 0; sword < 4; ++sword)
        {
            const bool upper = sword % 2 == 0;
            const float angle = (pulse * 4 + sword) * 2.399963f;
            const float radius = 35.f + 225.f * std::sqrt(((pulse * 7 + sword * 13) % 37) / 37.f);
            const Point floor = Polar(center, radius, angle, SceneCamera::GroundDepthScale());
            const Point sky = Polar({center.x, center.y - SkyHeight}, radius, angle, .34f);
            const Point from = upper ? sky : floor, to = upper ? floor : sky;
            MageUltimateSwordView::Draw(
                r,
                {from.x + (to.x - from.x) * travel, from.y + (to.y - from.y) * travel},
                upper ? 1.f : -1.f,
                alpha,
                upper,
                progress,
                pulse * 4 + sword);
        }
    }
    if (!gpuEnergy && merge > 0.f && t < MageUltimate::ExplosionTime)
    {
        const Point core{center.x, center.y - SkyHeight * .5f};
        r.Ellipse(core.x,
                  core.y,
                  8.f + 36.f * merge,
                  8.f + 36.f * merge,
                  Color(.67f, .77f, 1.f, merge * .16f));
        Ring(r, core, 15.f + 45.f * merge, 1.f, 1.8f, Color(.8f, .91f, 1.f, merge * .75f));
    }
    if (!gpuEnergy && t >= MageUltimate::ExplosionTime)
    {
        const float age = t - static_cast<float>(MageUltimate::ExplosionTime);
        const float expansion = Ease(age / .40f), fade = 1.f - Ease(age / .55f);
        const Point core{center.x, center.y - SkyHeight * .5f};
        r.Ellipse(core.x,
                  core.y,
                  35.f + 180.f * expansion,
                  35.f + 130.f * expansion,
                  Color(.52f, .65f, 1.f, .18f * fade));
        r.Ellipse(core.x,
                  core.y,
                  12.f + 55.f * expansion,
                  12.f + 55.f * expansion,
                  Color(.91f, .96f, 1.f, .85f * (1.f - Ease(age / .22f))));
        Ring(r,
             core,
             30.f + 340.f * expansion,
             .62f,
             5.f * (1.f - expansion) + 1.f,
             Color(.64f, .89f, 1.f, .9f * fade));
        Ring(r,
             center,
             MageUltimate::Radius * expansion,
             SceneCamera::GroundDepthScale(),
             4.f,
             Color(.69f, .48f, 1.f, .7f * fade));
        for (int i = 0; i < 18; ++i)
        {
            const float a = i * 2.f * Pi / 18.f;
            Line(r,
                 Polar(core, 20.f + 70.f * expansion, a, .7f),
                 Polar(core, 80.f + 220.f * expansion, a, .7f),
                 1.5f * fade,
                 Color(.71f, .84f, 1.f, fade * .65f));
        }
    }
}

void MageUltimateView::DrawCaster(Renderer& r, const MageUltimate& skill, Point feet)
{
    const float pose = (std::min)(330.f, skill.Time() * 1000.f);
    r.CombatVisual("mage-3", pose, feet.x, feet.y, skill.FaceLeft(), .35f, false);
}
