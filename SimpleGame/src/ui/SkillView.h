#pragma once

class Renderer;
class SkillSystem;

// Reads SkillSystem state and renders HUD/window only; it never changes skill data.
class SkillView
{
  public:
    void Draw(
        Renderer& renderer, const SkillSystem& skills, int width, int height, int level) const;
};
