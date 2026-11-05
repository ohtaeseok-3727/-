#pragma once

#include "HudTextLayout.h"
#include <algorithm>

// Shared visual language. Geometry uses the existing UI batch and text cache.
class GameUiTheme
{
  public:
    static Color Gold()
    {
        return Color(.86f, .38f, .08f);
    }

    static Color Text()
    {
        return Color(.16f, .23f, .31f);
    }

    static Color Muted()
    {
        return Color(.36f, .43f, .50f);
    }

    static Color Ink()
    {
        return Color(.98f, .98f, .94f, .98f);
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
        r.Rect(x + 3, y + 4, w, h, Color(.08f, .15f, .24f, .22f));
        r.Rect(x, y, w, h, Color(.40f, .52f, .63f));
        r.Rect(x + 1, y + 1, w - 2, h - 2, Color(.79f, .86f, .91f));
        r.Rect(x + 3, y + 3, w - 6, h - 6, Ink());
        r.Rect(x + 4, y + 4, w - 8, 2, Color(1, 1, 1));
        r.Rect(x + 4, y + h - 6, w - 8, 2, Color(.86f, .88f, .84f));
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
        r.Rect(x, y, w, h, hover && enabled ? Gold() : Color(.39f, .52f, .63f));
        r.Rect(x + 1,
               y + 1,
               w - 2,
               h - 2,
               enabled ? (pressed ? Color(.16f, .44f, .69f) : Color(.29f, .62f, .86f))
                       : Color(.76f, .80f, .81f));
        r.Rect(x + 2,
               y + 2,
               w - 4,
               (h - 4) * .48f,
               enabled ? (pressed ? Color(.24f, .54f, .77f) : Color(.48f, .77f, .94f))
                       : Color(.85f, .87f, .87f));
        const auto label = HudTextLayout::Fit(r, text, w - 8);
        Center(r,
               x + w * .5f,
               y + h * .5f + 5 + (pressed ? 1 : 0),
               label,
               enabled ? Color(.08f, .20f, .32f) : Muted());
    }

    static void Window(Renderer& r, float x, float y, float w, float h, const std::string& title)
    {
        Panel(r, x, y, w, h);
        r.Rect(x + 3, y + 3, w - 6, 31, Color(.96f, .50f, .14f));
        r.Rect(x + 4, y + 4, w - 8, 13, Color(1.f, .66f, .29f));
        r.Text(x + 12, y + 24, HudTextLayout::Fit(r, title, w - 52), Color(.25f, .15f, .05f));
        Button(r, x + w - 28, y + 6, 20, 20, "×");
    }

    static void Slot(Renderer& r, float x, float y, float size, bool selected = false)
    {
        r.Rect(x, y, size, size, selected ? Gold() : Color(.66f, .74f, .77f));
        r.Rect(x + 1, y + 1, size - 2, size - 2, Color(1, 1, 1));
        r.Rect(x + 3,
               y + 3,
               size - 6,
               size - 6,
               selected ? Color(1, .94f, .77f) : Color(.90f, .94f, .94f));
        r.Rect(x + 3, y + 3, size - 6, 2, Color(.80f, .87f, .89f));
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
        r.Rect(x, y, w, h, Color(.50f, .58f, .63f));
        r.Rect(x + 1, y + 1, w - 2, h - 2, Color(.87f, .90f, .91f));
        r.Rect(x + 1, y + 1, (w - 2) * ratio, h - 2, color);
        r.Rect(x + 1, y + 1, (w - 2) * ratio, 2, Color(1, 1, 1, .17f));
    }
};
