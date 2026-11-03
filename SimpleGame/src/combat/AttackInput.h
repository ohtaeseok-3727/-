#pragma once

// Track the Ctrl press edge and suppress held input when control resumes.
class AttackInput
{
  public:
    bool Poll(bool enabled, bool down);
    void Suspend();

  private:
    bool m_WasDown = false;
    bool m_WasEnabled = false;
};
