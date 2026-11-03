#pragma once
#include "Renderer.h"

class HudTextLayout
{
  public:
    static std::string Fit(Renderer& renderer, std::string text, float width)
    {
        if (width <= 0.f)
        {
            return {};
        }
        if (renderer.TextWidth(text) <= width)
        {
            return text;
        }
        const std::string suffix = "…";
        if (renderer.TextWidth(suffix) > width)
        {
            return {};
        }
        while (!text.empty() && renderer.TextWidth(text + suffix) > width)
        {
            std::size_t last = text.size() - 1;
            while (last > 0 && (static_cast<unsigned char>(text[last]) & 0xC0) == 0x80)
            {
                --last;
            }
            text.resize(last);
        }
        return text + suffix;
    }
};
