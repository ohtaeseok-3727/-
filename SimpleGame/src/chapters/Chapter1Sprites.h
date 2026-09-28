#pragma once
#include "Renderer.h"
#include <array>

enum class ChapterSprite
{
    GoblinWalk,
    GoblinAttack,
    Civilian,
    Healer,
    Commander,
    Hero,
    Count
};

// Owns optional chapter atlas resources; source PNGs are read-only.
class Chapter1Sprites
{
  public:
    ~Chapter1Sprites();
    Chapter1Sprites() = default;
    Chapter1Sprites(const Chapter1Sprites&) = delete;
    Chapter1Sprites& operator=(const Chapter1Sprites&) = delete;
    bool Draw(Renderer& renderer,
              ChapterSprite sprite,
              int frame,
              Point foot,
              float height,
              bool flip = false);

  private:
    struct Frame
    {
        int left = 0, top = 0, right = 0, bottom = 0;
    };

    struct Atlas
    {
        bool attempted = false;
        GLuint texture = 0;
        int width = 0, height = 0;
        std::vector<Frame> frames;
    };

    bool Load(ChapterSprite sprite, Atlas& atlas);
    std::array<Atlas, static_cast<std::size_t>(ChapterSprite::Count)> m_Atlases;
};
