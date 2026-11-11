#pragma once
#include "Renderer.h"

enum class Chapter3Prop
{
    Manuscript,
    Foundation,
    Record,
    Crate,
    Cart,
    Brake,
    Wagon,
    Permit
};

// Read-only props share one Renderer texture cache entry.
class Chapter3Props
{
  public:
    static void Draw(Renderer& renderer, Chapter3Prop prop, Point foot, float height);
};
