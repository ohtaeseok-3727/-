#pragma once

class SkillDragController
{
  public:
    void Begin(int skillIndex, int x, int y);
    void Move(int x, int y);
    void Cancel();
    int SkillIndex() const;
    int MouseX() const;
    int MouseY() const;

  private:
    int m_SkillIndex = -1;
    int m_MouseX = 0;
    int m_MouseY = 0;
};
