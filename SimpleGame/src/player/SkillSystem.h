#pragma once

#include "PlayerSkills.h"
#include "SkillDragController.h"
#include "SkillQuickSlots.h"
#include "SkillView.h"
#include "SkillLayout.h"
#include <string>
#include <unordered_map>

class PlayerProgression;
class Renderer;

struct SkillUseRequest
{
    std::string skillId;
    float calculatedDamage = 0.f;
    int projectileCount = 1;
};

class SkillSystem
{
  public:
    void Reset(int profession);
    void Update(float dt);
    void ToggleWindow();
    void CloseWindow();
    bool IsWindowOpen() const;
    bool IsSlotBound(char key) const;
    bool TryUse(char key, PlayerProgression& progression, std::string& notice, int attackBonus = 0);
    SkillUseRequest BuildRequest(const SkillData& skill,
                                 const PlayerProgression& progression,
                                 int attackBonus = 0) const;
    const SkillUseRequest& LastRequest() const;
    void MouseButton(bool down, int x, int y, int width, int height, int level);
    void MouseMove(int x, int y, int width, int height);
    void CancelDrag();
    void Draw(Renderer& renderer, int width, int height, int level) const;
    const PlayerSkills& PlayerSkillsState() const;
    const SkillQuickSlots& QuickSlots() const;
    const SkillDragController& Drag() const;
    const std::unordered_map<std::string, float>& Cooldowns() const;

  private:
    int SlotAt(int x, int y, int width, int height) const;
    int IconAt(int x, int y, int width, int height, int level) const;
    bool CanBind(const SkillData& skill, int level) const;
    PlayerSkills m_PlayerSkills;
    bool m_WindowOpen = false;
    SkillQuickSlots m_QuickSlots;
    SkillDragController m_Drag;
    SkillView m_View;
    std::unordered_map<std::string, float> m_Cooldowns;
    SkillUseRequest m_LastRequest;
};
