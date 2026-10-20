#pragma once
#include "Renderer.h"
#include <array>

struct FieldEnemy;

// Chapter-local texture ownership and frame selection; source images remain unchanged.
class Chapter2Sprites
{
  public:
    Chapter2Sprites() = default;
    ~Chapter2Sprites();
    Chapter2Sprites(const Chapter2Sprites&) = delete;
    Chapter2Sprites& operator=(const Chapter2Sprites&) = delete;

    void DrawEnemy(Renderer& renderer, const FieldEnemy& enemy, Point foot) const;
    void DrawHero(Renderer& renderer,
                  Point foot,
                  float time,
                  bool attacking = false,
                  bool faceLeft = false) const;

  private:
    enum class Sheet
    {
        Common,
        Elite,
        BarkanAttack,
        BarkanSupport,
        CommonWalk,
        EliteWalk,
        BarkanWalk,
        Hero,
        Count
    };

    struct Frame
    {
        int left = 0, top = 0, right = 0, bottom = 0;
        float pivotX = 0, pivotY = 0;
    };

    struct Atlas
    {
        bool attempted = false;
        GLuint texture = 0;
        int width = 0, height = 0;
        float referenceHeight = 1;
        std::vector<Frame> frames;
    };

    bool Load(Sheet sheet, Atlas& atlas) const;
    bool Draw(Renderer& renderer,
              Sheet sheet,
              int frame,
              Point foot,
              float height,
              bool flip = false) const;

    mutable std::array<Atlas, static_cast<std::size_t>(Sheet::Count)> m_Atlases;
};
