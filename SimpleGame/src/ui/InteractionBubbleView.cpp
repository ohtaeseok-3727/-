#include "stdafx.h"
#include "InteractionBubbleView.h"
#include <algorithm>
#include <sstream>

void InteractionBubbleView::DrawPrompt(
    Renderer& renderer, const std::string& text, Point anchor, int width, int height) const
{
    const float maximumWidth = (std::min)(220.f, (std::max)(1.f, float(width) - 48.f));
    Draw(renderer,
         Wrap(renderer, text, maximumWidth),
         anchor,
         width,
         height,
         Color(.96f, .86f, .58f),
         Color(.055f, .070f, .075f, .94f),
         Color(.63f, .52f, .31f, .95f));
}

void InteractionBubbleView::DrawDescription(
    Renderer& renderer, const std::string& text, Point anchor, int width, int height) const
{
    const float maximumWidth = (std::min)(360.f, (std::max)(1.f, float(width) - 48.f));
    Draw(renderer,
         Wrap(renderer, text, maximumWidth),
         anchor,
         width,
         height,
         Color(.93f, .91f, .79f),
         Color(.035f, .050f, .055f, .97f),
         Color(.76f, .65f, .42f, .98f));
}

std::vector<std::string> InteractionBubbleView::Wrap(
    Renderer& renderer, const std::string& text, float maximumWidth) const
{
    std::istringstream words(text);
    std::vector<std::string> lines;
    std::string word;
    std::string line;
    while (words >> word)
    {
        if (renderer.TextWidth(word) > maximumWidth)
        {
            if (!line.empty())
            {
                lines.push_back(line);
                line.clear();
            }

            std::string fragment;
            for (std::size_t index = 0; index < word.size();)
            {
                const unsigned char first = static_cast<unsigned char>(word[index]);
                const std::size_t characterLength =
                    first < 0x80 ? 1 : (first & 0xE0) == 0xC0 ? 2
                                      : (first & 0xF0) == 0xE0 ? 3
                                                                 : 4;
                const std::string character = word.substr(index, characterLength);
                if (!fragment.empty() && renderer.TextWidth(fragment + character) > maximumWidth)
                {
                    lines.push_back(fragment);
                    fragment.clear();
                }
                fragment += character;
                index += characterLength;
            }
            line = fragment;
            continue;
        }
        const std::string candidate = line.empty() ? word : line + " " + word;
        if (!line.empty() && renderer.TextWidth(candidate) > maximumWidth)
        {
            lines.push_back(line);
            line = word;
        }
        else
        {
            line = candidate;
        }
    }
    if (!line.empty())
    {
        lines.push_back(line);
    }
    return lines;
}

void InteractionBubbleView::Draw(Renderer& renderer,
                                 const std::vector<std::string>& lines,
                                 Point anchor,
                                 int width,
                                 int height,
                                 Color textColor,
                                 Color fillColor,
                                 Color outlineColor) const
{
    if (lines.empty() || width <= 0 || height <= 0)
    {
        return;
    }

    constexpr float paddingX = 13.f;
    constexpr float paddingY = 10.f;
    constexpr float lineHeight = 22.f;
    constexpr float tailHeight = 9.f;
    constexpr float hudBottom = 42.f;
    constexpr float hudTop = 116.f;

    float textWidth = 0.f;
    for (const auto& line : lines)
    {
        textWidth = (std::max)(textWidth, renderer.TextWidth(line));
    }
    const float bubbleWidth = textWidth + paddingX * 2.f;
    const float bubbleHeight = paddingY * 2.f + lineHeight * static_cast<float>(lines.size());
    if (bubbleWidth > float(width) - 24.f ||
        bubbleHeight + tailHeight > float(height) - hudTop - hudBottom - 20.f)
    {
        return;
    }
    const float leftLimit = 12.f;
    const float rightLimit = (std::max)(leftLimit, float(width) - bubbleWidth - 12.f);
    const float topLimit = hudTop + 10.f;
    const float bottomLimit =
        (std::max)(topLimit, float(height) - hudBottom - bubbleHeight - tailHeight - 10.f);
    const float x = (std::max)(leftLimit, (std::min)(anchor.x - bubbleWidth * .5f, rightLimit));
    const float y = (std::max)(topLimit,
                               (std::min)(anchor.y - bubbleHeight - tailHeight, bottomLimit));
    const float tailX = (std::max)(x + 12.f, (std::min)(anchor.x, x + bubbleWidth - 12.f));

    renderer.Rect(x - 1.f, y - 1.f, bubbleWidth + 2.f, bubbleHeight + 2.f, outlineColor);
    renderer.Rect(x, y, bubbleWidth, bubbleHeight, fillColor);
    renderer.Triangle({tailX - 8.f, y + bubbleHeight},
                      {tailX + 8.f, y + bubbleHeight},
                      {tailX, y + bubbleHeight + tailHeight},
                      outlineColor);
    renderer.Triangle({tailX - 6.f, y + bubbleHeight},
                      {tailX + 6.f, y + bubbleHeight},
                      {tailX, y + bubbleHeight + tailHeight - 2.f},
                      fillColor);
    for (std::size_t index = 0; index < lines.size(); ++index)
    {
        renderer.Text(x + paddingX,
                      y + paddingY + 15.f + lineHeight * static_cast<float>(index),
                      lines[index],
                      textColor);
    }
}
