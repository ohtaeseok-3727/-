#include "stdafx.h"
#include "SkillSystem.h"
#include "PlayerProgression.h"
#include <algorithm>
#include <cmath>

void SkillSystem::Reset(int profession)
{
    m_PlayerSkills.Reset(profession);
    m_WindowOpen = false;
    m_Drag.Cancel();
    m_QuickSlots.Reset();
    m_Cooldowns.clear();
    m_LastRequest = SkillUseRequest{};
}

void SkillSystem::Update(float dt)
{
    for (auto it = m_Cooldowns.begin(); it != m_Cooldowns.end();)
    {
        it->second = (std::max)(0.f, it->second - dt);
        if (it->second == 0.f)
        {
            it = m_Cooldowns.erase(it);
        }
        else
        {
            ++it;
        }
    }
}

void SkillSystem::ToggleWindow()
{
    m_WindowOpen = !m_WindowOpen;
    CancelDrag();
}

void SkillSystem::CloseWindow()
{
    m_WindowOpen = false;
    CancelDrag();
}

bool SkillSystem::IsWindowOpen() const
{
    return m_WindowOpen;
}

bool SkillSystem::IsSlotBound(char key) const
{
    return m_QuickSlots.IsBound(key);
}

bool SkillSystem::TryUse(char key,
                         PlayerProgression& progression,
                         std::string& notice,
                         int attackBonus)
{
    m_LastRequest = SkillUseRequest{};
    const std::string& skillId = m_QuickSlots.ForKey(key);
    if (skillId.empty())
    {
        return false;
    }
    const SkillData* skill = m_PlayerSkills.Find(skillId);
    if (!skill)
    {
        return true;
    }
    if (!m_PlayerSkills.CanUse(*skill, progression.Level()))
    {
        notice = skill->name + " : 아직 사용할 수 없습니다";
        return true;
    }
    if (m_Cooldowns.count(skill->id))
    {
        notice = skill->name + " : 재사용 대기 중";
        return true;
    }
    if (!progression.SpendMana(skill->manaCost))
    {
        notice = skill->name + " : 마나가 부족합니다";
        return true;
    }
    m_Cooldowns[skill->id] = skill->cooldown;
    m_LastRequest = BuildRequest(*skill, progression, attackBonus);
    notice = skill->name + " 사용 · 개별 피해 " +
             std::to_string(static_cast<int>(std::lround(m_LastRequest.calculatedDamage))) +
             " · 발사체 " + std::to_string(m_LastRequest.projectileCount) + "개";
    return true;
}

SkillUseRequest SkillSystem::BuildRequest(const SkillData& skill,
                                          const PlayerProgression& progression,
                                          int attackBonus) const
{
    SkillUseRequest request;
    request.skillId = skill.id;
    const int stat = skill.profession == 0 ? progression.Intelligence() : progression.Strength();
    request.calculatedDamage =
        (progression.Attack() + attackBonus) * (skill.attackRatio / 100.f) + stat * skill.statRatio;
    request.projectileCount = skill.projectileCount;
    if (skill.id == "mage_wind_blades")
    {
        request.projectileCount =
            (std::max)(1, (std::min)(3, progression.Level() - skill.requiredLevel + 1));
    }
    return request;
}

const SkillUseRequest& SkillSystem::LastRequest() const
{
    return m_LastRequest;
}

void SkillSystem::MouseButton(bool down, int x, int y, int width, int height, int level)
{
    m_Drag.Move(x, y);
    if (down)
    {
        if (m_WindowOpen)
        {
            m_Drag.Begin(IconAt(x, y, width, height, level), x, y);
        }
        return;
    }
    if (m_Drag.SkillIndex() >= 0)
    {
        const auto& skills = m_PlayerSkills.Available();
        const int slot = SlotAt(x, y, width, height);
        if (slot >= 0 && CanBind(skills[m_Drag.SkillIndex()], level))
        {
            m_QuickSlots.Assign(slot, skills[m_Drag.SkillIndex()].id);
        }
    }
    CancelDrag();
}

void SkillSystem::MouseMove(int x, int y, int, int)
{
    m_Drag.Move(x, y);
}

void SkillSystem::CancelDrag()
{
    m_Drag.Cancel();
}

int SkillSystem::SlotAt(int x, int y, int width, int height) const
{
    return SkillLayout::Create(width, height).SlotAt(float(x), float(y));
}

int SkillSystem::IconAt(int x, int y, int width, int height, int level) const
{
    const auto& skills = m_PlayerSkills.Available();
    const SkillLayout layout = SkillLayout::Create(width, height);
    if (!layout.canShowWindow)
    {
        return -1;
    }
    for (int i = 0; i < static_cast<int>(skills.size()); ++i)
    {
        const float iconX = layout.CardX(i) + 22.f;
        const float iconY = layout.CardY(i) + 28.f;
        if (x >= iconX - 24.f && x <= iconX + 24.f && y >= iconY - 24.f && y <= iconY + 24.f &&
            CanBind(skills[i], level))
        {
            return i;
        }
    }
    return -1;
}

bool SkillSystem::CanBind(const SkillData& skill, int level) const
{
    return m_PlayerSkills.CanUse(skill, level);
}

void SkillSystem::Draw(Renderer& renderer, int width, int height, int level) const
{
    m_View.Draw(renderer, *this, width, height, level);
}

const PlayerSkills& SkillSystem::PlayerSkillsState() const
{
    return m_PlayerSkills;
}

const SkillQuickSlots& SkillSystem::QuickSlots() const
{
    return m_QuickSlots;
}

const SkillDragController& SkillSystem::Drag() const
{
    return m_Drag;
}

const std::unordered_map<std::string, float>& SkillSystem::Cooldowns() const
{
    return m_Cooldowns;
}
