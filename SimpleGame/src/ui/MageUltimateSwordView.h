#pragma once
#include "Renderer.h"
#include <algorithm>
#include <cmath>

// Displays the Blender turntable without introducing damage/projectile objects.
class MageUltimateSwordView
{
  public:
    static void Draw(Renderer& renderer,
                     Point tip,
                     float direction,
                     float alpha,
                     bool upper,
                     float progress,
                     int seed)
    {
        constexpr float Scale = 1.5f;
        constexpr float Frames = 16.f;
        const int frame = (seed * 3 + static_cast<int>(progress * 12.f)) % 16;
        const float bank = std::sin(seed * 2.399963f) * .14f * (1.f - progress);
        const auto point = [&](float x, float y)
        {
            const float localX = (x - 16.f) * Scale;
            const float localY = (5.f - y) * direction * Scale;
            return Point{std::round(tip.x + localX + bank * localY), std::round(tip.y + localY)};
        };
        // Small stepped afterimages preserve the pixel grid instead of blurring it.
        const Color trail =
            upper ? Color(.20f, .68f, 1.f, alpha * .3f) : Color(.16f, .45f, 1.f, alpha * .3f);
        for (int i = 0; i < 4; ++i)
        {
            const float y = std::round(tip.y - direction * (88.f + i * 6.f));
            renderer.Rect(std::round(tip.x) - 1.5f,
                          y,
                          3.f,
                          3.f,
                          Color(trail.r, trail.g, trail.b, trail.a * (4 - i) / 4.f));
        }
        renderer.ImageQuad(L"../Effects/MageSwordSong/sword-turntable.png",
                           point(0, 0),
                           point(32, 0),
                           point(32, 64),
                           point(0, 64),
                           alpha,
                           {frame / Frames, 0.f},
                           {(frame + 1) / Frames, 1.f});
    }
};
