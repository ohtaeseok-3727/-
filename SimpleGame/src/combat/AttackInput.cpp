#include "stdafx.h"
#include "AttackInput.h"

bool AttackInput::Poll(bool enabled, bool down)
{
    const bool pressed = enabled && m_WasEnabled && down && !m_WasDown;
    m_WasDown = down;
    m_WasEnabled = enabled;
    return pressed;
}

void AttackInput::Suspend()
{
    // Resuming while Ctrl is held must not create an artificial press.
    m_WasEnabled = false;
}
