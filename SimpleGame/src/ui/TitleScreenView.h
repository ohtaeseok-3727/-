#pragma once
#include "Renderer.h"

class TitleScreenView
{
  public:
    void Draw(Renderer& renderer, int width, int height) const;
    void MouseMove(int x, int y, int width, int height);
    int MouseButton(bool down, int x, int y, int width, int height);
    void CancelPress();

  private:
    int m_Hovered = -1;
    int m_Pressed = -1;
};
