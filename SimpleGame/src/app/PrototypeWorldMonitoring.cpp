#include "stdafx.h"
#include "PrototypeWorld.h"

MonitorSnapshot PrototypeWorld::CaptureMonitoring() const
{
    if (!m_Started)
    {
        return {};
    }
    auto snapshot = m_Level.CaptureMonitoring(m_Map, {m_X, m_Y});
    snapshot.status = m_Level.IsDefeated() ? "defeated"
                      : m_Paused           ? "paused"
                      : MenuOpen()         ? "menu"
                                           : "running";
    return snapshot;
}
