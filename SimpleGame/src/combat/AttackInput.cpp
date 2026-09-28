#include "stdafx.h"
#include "AttackInput.h"
#include <Windows.h>

bool AttackInput::Poll(bool enabled)
{
    const bool down = (GetAsyncKeyState(VK_CONTROL) & 0x8000) != 0;
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
