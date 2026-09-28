#pragma once

#include "Renderer.h"
#include <string>
#include <vector>

// Draws camera-projected landmark prompts and descriptions without owning game state.
class InteractionBubbleView
{
  public:
    void DrawPrompt(Renderer& renderer,
                    const std::string& text,
                    Point anchor,
                    int width,
                    int height) const;
    void DrawDescription(Renderer& renderer,
                         const std::string& text,
                         Point anchor,
                         int width,
                         int height) const;

  private:
    std::vector<std::string> Wrap(Renderer& renderer,
                                  const std::string& text,
                                  float maximumWidth) const;
    void Draw(Renderer& renderer,
              const std::vector<std::string>& lines,
              Point anchor,
              int width,
              int height,
              Color textColor,
              Color fillColor,
              Color outlineColor) const;
};
