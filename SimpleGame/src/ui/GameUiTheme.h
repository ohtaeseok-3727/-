#pragma once

#include "HudTextLayout.h"
#include <algorithm>

// Shared visual language. Geometry uses the existing UI batch and text cache.
class GameUiTheme
{
  public:
    static Color Gold()
    {
        return Color(.77f, .63f, .36f);
    }

    static Color Text()
    {
        return Color(.91f, .89f, .80f);
    }

    static Color Muted()
    {
        return Color(.55f, .63f, .65f);
    }

    static Color Ink()
    {
        return Color(.025f, .038f, .049f, .96f);
    }

    static void Diamond(Renderer& r, float x, float y, float size, Color color)
    {
        r.Quad({x, y - size}, {x + size, y}, {x, y + size}, {x - size, y}, color);
    }

    static void Panel(Renderer& r, float x, float y, float w, float h)
    {
        if (w <= 8.f || h <= 8.f)
        {
            return;
        }
        r.Rect(x + 4, y + 5, w, h, Color(0, 0, 0, .32f));
        r.Rect(x, y, w, h, Gold());
        r.Rect(x + 1, y + 1, w - 2, h - 2, Ink());
        r.Rect(x + 5, y + 5, w - 10, 1, Color(.32f, .32f, .25f));
        r.Rect(x + 5, y + h - 6, w - 10, 1, Color(.21f, .25f, .25f));
        for (const float edge : {x + 1, x + w - 1})
        {
            Diamond(r, edge, y + 1, 3, Gold());
            Diamond(r, edge, y + h - 1, 3, Gold());
        }
    }

    static void Center(Renderer& r,
                       float x,
                       float baseline,
                       const std::string& text,
                       Color color,
                       float scale = 1.f)
    {
        r.Text(x - r.TextWidth(text) * scale * .5f, baseline, text, color, scale);
    }

    static void Meter(
        Renderer& r, float x, float y, float w, float h, int value, int maximum, Color color)
    {
        const float ratio = std::clamp(float(value) / (std::max)(1, maximum), 0.f, 1.f);
        r.Rect(x, y, w, h, Color(.20f, .23f, .23f));
        r.Rect(x + 1, y + 1, w - 2, h - 2, Color(.045f, .055f, .06f));
        r.Rect(x + 1, y + 1, (w - 2) * ratio, h - 2, color);
        r.Rect(x + 1, y + 1, (w - 2) * ratio, 2, Color(1, 1, 1, .17f));
    }
};
