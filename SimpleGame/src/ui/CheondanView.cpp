#include "stdafx.h"
#include "CheondanView.h"
#include "KnightSkills.h"
#include "KnightSkillData.h"
#include <algorithm>
#include <cmath>

namespace
{
    constexpr float Pi = 3.14159265f;

    float Ease(float x)
    {
        x = std::clamp(x, 0.f, 1.f);
        return x * x * (3.f - 2.f * x);
    }

    Point Mix(Point a, Point b, float t)
    {
        return {a.x + (b.x - a.x) * t, a.y + (b.y - a.y) * t};
    }

    void Stroke(Renderer& r, Point a, Point b, float width, Color color)
    {
        const float length = std::hypot(b.x - a.x, b.y - a.y);
        if (length < .001f)
            return;
        const Point n{(b.y - a.y) * width / length, -(b.x - a.x) * width / length};
        r.Quad({a.x + n.x, a.y + n.y},
               {b.x + n.x, b.y + n.y},
               {b.x - n.x, b.y - n.y},
               {a.x - n.x, a.y - n.y},
               color);
    }

    // Stable jagged edges: no per-frame random flicker, no detached fragments.
    float Edge(float u)
    {
        return .65f * std::sin(u * 97.f) + .35f * std::sin(u * 173.f + 1.3f);
    }

    void Blade(Renderer& r, Point root, Point tip, float width, float alpha)
    {
        const float length = std::hypot(tip.x - root.x, tip.y - root.y);
        if (length < .01f)
            return;
        const Point n{-(tip.y - root.y) / length, (tip.x - root.x) / length};
        const Color layers[] = {{1.f, .54f, .08f, alpha * .10f},
                                {1.f, .73f, .20f, alpha * .28f},
                                {1.f, .87f, .49f, alpha * .9f},
                                {1.f, .99f, .91f, alpha}};
        const float widths[] = {1.8f, 1.35f, 1.f, .60f};
        for (int layer = 0; layer < 4; ++layer)
        {
            for (int i = 0; i < 28; ++i)
            {
                const auto pair = [&](float u, float side)
                {
                    const auto p = Mix(root, tip, u);
                    const float taper = (1.f - u) * (.7f + .3f * std::sin(u * Pi));
                    const float w = width * widths[layer] * taper * (1.f + .1f * Edge(u));
                    return Point{p.x + n.x * w * side, p.y + n.y * w * side};
                };
                const float a = i / 28.f, b = (i + 1) / 28.f;
                r.Quad(pair(a, -1), pair(b, -1), pair(b, 1), pair(a, 1), layers[layer]);
            }
        }
        Stroke(r, root, tip, 1.f, Color(1.f, 1.f, .97f, alpha));
    }

    struct CutLine
    {
        Point a, b;
    };

    CutLine ScreenLine(int width, int height, bool left)
    {
        const float ax = width * .19f, bx = width * .84f;
        return {{left ? width - ax : ax, height * .13f}, {left ? width - bx : bx, height * .91f}};
    }

    float FractureFade(float t)
    {
        return 1.f - Ease((t - 1.50f) / .39f);
    }

    // One connected rough ribbon; widening toward the viewer conveys depth.
    void Ribbon(Renderer& r, CutLine line, float width, float progress, Color color)
    {
        const float length = std::hypot(line.b.x - line.a.x, line.b.y - line.a.y);
        if (length < 1.f || progress <= 0.f || color.a <= 0.f)
            return;
        const Point n{-(line.b.y - line.a.y) / length, (line.b.x - line.a.x) / length};
        for (int i = 0; i < 96; ++i)
        {
            const auto point = [&](float u, float side)
            {
                auto p = Mix(line.a, line.b, u);
                const float taper = std::pow((std::max)(0.f, std::sin(Pi * u)), .48f);
                const float w = width * taper * (.35f + .85f * u) * (1.f + .12f * Edge(u));
                const float offset = Edge(u) * 3.f + side * w;
                return Point{p.x + n.x * offset, p.y + n.y * offset};
            };
            const float a = i / 96.f, b = (std::min)((i + 1) / 96.f, progress);
            if (a >= progress)
                break;
            r.Quad(point(a, -1), point(b, -1), point(b, 1), point(a, 1), color);
        }
    }

}

void CheondanView::DrawCast(Renderer& r, const KnightSkills& skills, Point feet)
{
    const float t = skills.CastTime();
    const float charge = Ease(t / KnightSkillData::UltimateCharge);
    const float swing = Ease((t - KnightSkillData::UltimateCharge) /
                             (KnightSkillData::UltimateSwingEnd - KnightSkillData::UltimateCharge));
    const float sign = skills.FaceLeft() ? -1.f : 1.f;
    // Reuse the existing knight's body animation only; all new effect geometry is procedural.
    const float poseMs = t < KnightSkillData::UltimateCharge ? 150.f + 110.f * charge
                         : t < KnightSkillData::UltimateSwingEnd
                             ? 260.f + 95.f * swing
                             : 355.f + (t - KnightSkillData::UltimateSwingEnd) * 700.f;
    r.CombatVisual(
        "knight-3", (std::min)(poseMs, 809.f), feet.x, feet.y, skills.FaceLeft(), .32f, false);
    const Point raised{feet.x - 6.f * sign, feet.y - 41.f};
    const Point lowered{feet.x + 15.f * sign, feet.y - 7.f};
    const Point hilt = Mix(raised, lowered, swing);
    const float angle = -2.05f + 2.62f * swing;
    const float length = (30.f + 245.f * charge) * (1.f - .30f * swing);
    const Point tip{hilt.x + sign * std::cos(angle) * length, hilt.y + std::sin(angle) * length};
    if (t < KnightSkillData::UltimateCharge)
    {
        // Curved streams converge into the blade instead of flying out as debris.
        for (int stream = 0; stream < 10; ++stream)
        {
            const float cycle = std::fmod(t * 1.7f + stream * .173f, 1.f);
            const float a = stream * 2.39996f;
            const Point start{raised.x + std::cos(a) * (100.f + 85.f * charge),
                              raised.y + std::sin(a) * (85.f + 60.f * charge)};
            const Point finish = Mix(hilt, tip, .2f + .055f * stream);
            for (int segment = 0; segment < 12; ++segment)
            {
                const auto path = [&](float u)
                {
                    auto p = Mix(start, finish, u);
                    p.x += sign * std::sin(u * Pi) * 25.f;
                    p.y -= std::sin(u * Pi) * 23.f;
                    return p;
                };
                const float u = std::clamp(cycle - segment * .025f, 0.f, 1.f);
                const float v = std::clamp(u - .025f, 0.f, 1.f);
                Stroke(r,
                       path(v),
                       path(u),
                       1.4f * (1.f - segment / 13.f),
                       Color(1.f, .81f, .36f, (1.f - cycle) * (.25f + .65f * charge)));
            }
        }
    }
    if (t < KnightSkillData::UltimateSwingEnd)
    {
        Blade(r, hilt, tip, 4.f + 13.f * charge + 30.f * swing, 1.f);
    }
}

void CheondanView::DrawWave(Renderer& r, const KnightSkillShot& shot, Point ground)
{
    const auto bounds = shot.Bounds();
    const float sign = shot.faceLeft ? -1.f : 1.f;
    const float age = shot.age - shot.delay;
    const float alpha = 1.f - Ease((age / shot.Duration() - .82f) / .18f);
    const float width = bounds.right - bounds.left;
    // World-space fracture front retains a visible cue for the unchanged moving damage bounds.
    const CutLine front{
        {ground.x + sign * (bounds.left + width * .18f), ground.y + bounds.top},
        {ground.x + sign * (bounds.right - width * .18f), ground.y + bounds.bottom}};
    Ribbon(r, front, width * .12f, 1.f, Color(1.f, .61f, .16f, alpha * .25f));
    Ribbon(r, front, width * .035f, 1.f, Color(1.f, .92f, .65f, alpha * .7f));
}

void CheondanView::PrepareScreen(Renderer& r, const KnightSkills& skills, int width, int height)
{
    const float t = skills.UltimateVisualTime();
    if (t < KnightSkillData::UltimateRelease || t >= 1.89f)
        return;
    const auto line = ScreenLine(width, height, skills.UltimateVisualFaceLeft());
    const float age = t - KnightSkillData::UltimateRelease;
    const float scale = (std::min)(width / 1280.f, height / 720.f);
    r.ScreenFracture() = {line.a.x,
                          line.a.y,
                          line.b.x,
                          line.b.y,
                          13.f * scale * Ease(age / .085f) * FractureFade(t),
                          Ease(age / .23f)};
}

void CheondanView::DrawScreen(Renderer& r, const KnightSkills& skills, int width, int height)
{
    const float t = skills.UltimateVisualTime();
    if (t < KnightSkillData::UltimateCharge || t >= 1.89f)
        return;
    const auto line = ScreenLine(width, height, skills.UltimateVisualFaceLeft());
    const float scale = (std::min)(width / 1280.f, height / 720.f);
    const float swing = Ease((t - KnightSkillData::UltimateCharge) /
                             (KnightSkillData::UltimateSwingEnd - KnightSkillData::UltimateCharge));
    if (t < KnightSkillData::UltimateSwingEnd)
    {
        // A foreshortened edge sweeps toward the camera; the held trail appears afterwards.
        const auto head = Mix(line.a, line.b, swing);
        const auto tail = Mix(line.a, line.b, (std::max)(0.f, swing - .20f));
        Blade(r, head, tail, (8.f + 43.f * swing) * scale, .95f);
        return;
    }
    const float fade = FractureFade(t);
    const float trail = (1.f - Ease((t - KnightSkillData::UltimateRelease) / .23f)) * .85f;
    Ribbon(r, line, 31.f * scale, 1.f, Color(1.f, .51f, .10f, .16f * trail));
    Ribbon(r, line, 15.f * scale, 1.f, Color(1.f, .80f, .38f, .58f * trail));
    Ribbon(r, line, 5.f * scale, 1.f, Color(1.f, .98f, .86f, trail));
    if (t < KnightSkillData::UltimateRelease)
        return;
    const float age = t - KnightSkillData::UltimateRelease;
    const float progress = Ease(age / .23f);
    // Dark open seam with luminous edges, never detached shards.
    Ribbon(r, line, 10.f * scale, progress, Color(1.f, .66f, .24f, .85f * fade));
    Ribbon(r, line, 6.f * scale, progress, Color(.012f, .018f, .033f, fade));
    const float length = std::hypot(line.b.x - line.a.x, line.b.y - line.a.y);
    const Point normal{-(line.b.y - line.a.y) / length, (line.b.x - line.a.x) / length};
    for (int branch = 0; branch < 15; ++branch)
    {
        const float u = .08f + branch * .059f;
        const float growth = Ease((progress - u) / .16f);
        if (growth <= 0.f)
            continue;
        auto root = Mix(line.a, line.b, u);
        root.x += normal.x * Edge(u) * 3.f;
        root.y += normal.y * Edge(u) * 3.f;
        const float side = branch % 2 ? -1.f : 1.f;
        const float reach = (42.f + 65.f * std::abs(std::sin(branch * 2.17f))) * scale;
        Point previous = root;
        for (int segment = 1; segment <= 5; ++segment)
        {
            const float v = segment / 5.f;
            const float offset = side * reach * v * growth;
            const Point next{root.x + normal.x * offset +
                                 std::sin(segment * 2.1f + branch) * 12.f * scale * v * growth,
                             root.y + normal.y * offset + v * 23.f * scale * growth};
            Stroke(
                r, previous, next, 2.5f * scale * (1.f - v * .7f), Color(.02f, .03f, .05f, fade));
            Stroke(r,
                   {previous.x + scale, previous.y},
                   {next.x + scale, next.y},
                   .8f * scale * (1.f - v * .6f),
                   Color(1.f, .88f, .61f, fade * .8f));
            previous = next;
        }
    }
}
