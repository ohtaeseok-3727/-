#pragma once
#include "Renderer.h"
class KnightSkills;
struct KnightSkillShot;
class BasicAttack;

class KnightSkillView
{
  public:
    static void DrawBody(Renderer& renderer, const KnightSkills& skills, Point feet);
    static void DrawShot(Renderer& renderer, const KnightSkillShot& shot, Point ground);
    static void DrawAura(Renderer& renderer,
                         const KnightSkills& skills,
                         const BasicAttack& attack,
                         Point feet,
                         bool faceLeft,
                         int walkFrame,
                         float time);
};
