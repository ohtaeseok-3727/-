#pragma once

#include "Renderer.h"
#include <array>

enum class Chapter3Person
{
    Elian,
    Edric,
    Courier,
    Guard,
    Serad,
    Hero,
    Count
};

enum class Chapter3Motion
{
    Idle,
    Walk,
    Attack,
    Ranged
};

// One immutable atlas per NPC; motion selection never changes combat or save state.
class Chapter3Sprites
{
  public:
    ~Chapter3Sprites();
    Chapter3Sprites() = default;
    Chapter3Sprites(const Chapter3Sprites&) = delete;
    Chapter3Sprites& operator=(const Chapter3Sprites&) = delete;
    bool Draw(Renderer& renderer,
              Chapter3Person person,
              Chapter3Motion motion,
              float seconds,
              Point foot,
              bool flip = false,
              float height = 52.f);
    static int Frame(Chapter3Motion motion, float seconds);

  private:
    struct Atlas
    {
        bool attempted = false;
        GLuint texture = 0;
    };

    bool Load(Chapter3Person person, Atlas& atlas);
    std::array<Atlas, static_cast<std::size_t>(Chapter3Person::Count)> m_Atlases;
};
