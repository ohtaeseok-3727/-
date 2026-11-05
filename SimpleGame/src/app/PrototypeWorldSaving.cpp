#include "stdafx.h"
#include "PrototypeWorld.h"
#include "GameSave.h"

void PrototypeWorld::SaveGame()
{
    GameSave::Save(*this, m_SaveMessage);
    if (m_Started)
    {
        m_Level.ShowSkillNotice(m_SaveMessage);
    }
}

void PrototypeWorld::LoadGame()
{
    GameSave::Load(*this, m_SaveMessage);
    if (m_Started)
    {
        m_Level.ShowSkillNotice(m_SaveMessage);
    }
}
