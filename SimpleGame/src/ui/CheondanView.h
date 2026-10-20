#pragma once
#include "Renderer.h"
class KnightSkills;
struct KnightSkillShot;

// Procedural geometry only: no effect textures or generated sprite sheets.
class CheondanView
{
  public:
    static void DrawCast(Renderer& renderer, const KnightSkills& skills, Point feet);
    static void DrawWave(Renderer& renderer, const KnightSkillShot& shot, Point ground);
    static void PrepareScreen(Renderer& renderer,
                              const KnightSkills& skills,
                              int width,
                              int height);
    static void DrawScreen(Renderer& renderer, const KnightSkills& skills, int width, int height);
};
