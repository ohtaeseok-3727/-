#include "stdafx.h"
#include "SkillDragController.h"

void SkillDragController::Begin(int skillIndex, int x, int y)
{
    m_SkillIndex = skillIndex;
    Move(x, y);
}

void SkillDragController::Move(int x, int y)
{
    m_MouseX = x;
    m_MouseY = y;
}

void SkillDragController::Cancel()
{
    m_SkillIndex = -1;
}

int SkillDragController::SkillIndex() const
{
    return m_SkillIndex;
}

int SkillDragController::MouseX() const
{
    return m_MouseX;
}

int SkillDragController::MouseY() const
{
    return m_MouseY;
}
