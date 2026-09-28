#pragma once

// Poll Ctrl separately because GLUT's normal character callback omits modifiers.
class AttackInput
{
  public:
    bool Poll(bool enabled);
    void Suspend();

  private:
    bool m_WasDown = false;
    bool m_WasEnabled = false;
};
