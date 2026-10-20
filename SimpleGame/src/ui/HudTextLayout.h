#pragma once
#include "Renderer.h"

class HudTextLayout
{
  public:
    static std::string Fit(Renderer& renderer, std::string text, float width)
    {
        if (renderer.TextWidth(text) <= width)
        {
            return text;
        }
        const std::string suffix = "…";
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
