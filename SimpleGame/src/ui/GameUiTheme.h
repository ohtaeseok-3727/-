#pragma once

#include "HudTextLayout.h"
#include <algorithm>

// Shared visual language. Geometry uses the existing UI batch and text cache.
class GameUiTheme
{
  public:
    static Color Gold()
    {
        return Color(.68f, .57f, .38f);
    }

    static Color Text()
    {
        return Color(.82f, .85f, .88f);
    }

    static Color Muted()
    {
        return Color(.58f, .64f, .69f);
    }

    static Color Ink()
    {
        return Color(.055f, .070f, .095f, .98f);
    }

    static Color Surface()
    {
        return Color(.085f, .11f, .145f);
    }

    static Color Border()
    {
        return Color(.24f, .30f, .36f);
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
        r.Rect(x + 3, y + 4, w, h, Color(0.f, 0.f, 0.f, .45f));
        r.Rect(x, y, w, h, Border());
        r.Rect(x + 1, y + 1, w - 2, h - 2, Color(.12f, .16f, .20f));
        r.Rect(x + 3, y + 3, w - 6, h - 6, Ink());
        r.Rect(x + 4, y + 4, w - 8, 2, Color(.18f, .23f, .29f));
        r.Rect(x + 4, y + h - 6, w - 8, 2, Color(.035f, .050f, .065f));
    }

    static void Button(Renderer& r,
                       float x,
                       float y,
                       float w,
                       float h,
                       const std::string& text,
                       bool hover = false,
                       bool pressed = false,
                       bool enabled = true)
    {
        if (w <= 4 || h <= 4)
        {
            return;
        }
        r.Rect(x, y, w, h, hover && enabled ? Gold() : Border());
        r.Rect(x + 1,
               y + 1,
               w - 2,
               h - 2,
               enabled ? (pressed ? Color(.070f, .10f, .14f) : Color(.12f, .16f, .21f))
                       : Color(.075f, .09f, .11f));
        r.Rect(x + 2,
               y + 2,
               w - 4,
               (h - 4) * .48f,
               enabled ? (pressed ? Color(.10f, .14f, .19f) : Color(.17f, .22f, .28f))
                       : Color(.09f, .11f, .135f));
        const auto label = HudTextLayout::Fit(r, text, w - 8);
        Center(
            r, x + w * .5f, y + h * .5f + 5 + (pressed ? 1 : 0), label, enabled ? Text() : Muted());
    }

    static void Window(Renderer& r, float x, float y, float w, float h, const std::string& title)
    {
        Panel(r, x, y, w, h);
        r.Rect(x + 3, y + 3, w - 6, 31, Color(.11f, .145f, .19f));
        r.Rect(x + 4, y + 4, w - 8, 13, Color(.15f, .19f, .24f));
        r.Text(x + 12, y + 24, HudTextLayout::Fit(r, title, w - 52), Text());
        Button(r, x + w - 28, y + 6, 20, 20, "×");
    }

    static void Slot(Renderer& r, float x, float y, float size, bool selected = false)
    {
        r.Rect(x, y, size, size, selected ? Gold() : Border());
        r.Rect(x + 1, y + 1, size - 2, size - 2, Color(.18f, .23f, .29f));
        r.Rect(x + 3, y + 3, size - 6, size - 6, selected ? Color(.15f, .14f, .105f) : Surface());
        r.Rect(x + 3, y + 3, size - 6, 2, Color(.18f, .22f, .27f));
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
        r.Rect(x, y, w, h, Border());
        r.Rect(x + 1, y + 1, w - 2, h - 2, Color(.035f, .050f, .070f));
        r.Rect(x + 1, y + 1, (w - 2) * ratio, h - 2, color);
        r.Rect(x + 1, y + 1, (w - 2) * ratio, 2, Color(.65f, .71f, .77f, .10f));
    }
};
