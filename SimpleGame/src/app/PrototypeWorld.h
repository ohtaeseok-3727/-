#pragma once
#include "Renderer.h"
#include "BasicAttack.h"
#include "PlayerStateMachine.h"
#include "TitleScreenView.h"
#include "InteractionBubbleView.h"
#include "LevelMap.h"
#include "GameSession.h"
#include "GameView.h"
#include "SkillSystem.h"
#include "MageProjectiles.h"
#include "MageUltimate.h"
#include "KnightSkills.h"
#include "MapView.h"
#include "SceneGraph.h"
#include "SceneRenderer.h"
#include "ExpeditionView.h"
#include "SettingsMenu.h"
#include "TradeSystem.h"
#include "GameUiControls.h"
#include <utility>
#include <vector>

class PrototypeWorld
{
  public:
    void Update(float dt,
                float dx,
                float dy,
                bool sprint,
                bool attackPressed,
                bool interactionHeld = false);
    void Draw(Renderer& renderer, int width, int height);

    void Select(int value);
    void TitleMouseButton(bool down, int x, int y);
    void TitleMouseMove(int x, int y);
    void EnableChapterPreview(const std::string& chapter);

    void Reset();
    void DebugLevel20();
    void ToggleInventory();
    void ChangeInventoryPage(int direction);
    void ToggleMap();
    void InspectLandmark();
    void ToggleSkills();
    bool HandleSkillKey(char key);
    bool IsSkillBound(char key) const;
    void SkillMouseButton(bool down, int x, int y);
    void SkillMouseMove(int x, int y);
    void CancelSkillDrag();
    bool IsSkillWindowOpen() const;
    bool MenuOpen() const;
    bool CloseMenus();
    void ToggleSystemMenu();
    bool SystemMenuOpen() const;
    bool SystemMenuMouseButton(
        bool down, int x, int y, int width, int height, PostSettings& settings);
    void SystemMenuMouseMove(int x, int y);
    void CancelMenuPress();
    void ToggleJournal();
    void ReturnToCamp();
    void RetryDefeat();
    void SaveGame();
    void LoadGame();
    void ToggleTrade();
    void UsePotion(bool mana);
    bool UiMouseButton(bool down, int x, int y, int width, int height);
    bool ConsumeUiResume();

    void SetPaused(bool value, bool focused = true)
    {
        if (value && !m_Paused)
        {
            m_Attack.Reset();
            m_UiControls.Cancel();
            m_WorldInteractionHeld = false;
        }

        m_Paused = value;
        m_InputFocused = focused;
    }

    bool Started() const
    {
        return m_Started;
    }

    MonitorSnapshot CaptureMonitoring() const;

  private:
    friend class GameSave;
    friend class VisualPlaytest;
    void UpdatePlayerState();
    void Stream();
    GameUiControls::State UiState() const;
    void ApplyUiAction(GameUiControls::Action action);
    bool Blocked(float x, float y) const;
    Point Project(float x, float y) const;
    float m_X = 0, m_Y = 0, m_CameraX = 0, m_CameraY = 0;
    float m_Time = 0, m_Walk = 0;
    int m_Width = 1280, m_Height = 720, m_Class = 1;
    bool m_Started = false, m_Paused = false;
    bool m_InputFocused = true;
    bool m_FaceLeft = false; // Vertical movement retains the last horizontal facing.
    bool m_Moving = false;
    SceneGraph m_Scene;
    SceneRenderer m_SceneRenderer;
    TitleScreenView m_TitleScreen;
    BasicAttack m_Attack;
    PlayerStateMachine m_PlayerState;
    LevelMap m_Map;
    GameSession m_Level;
    GameView m_LevelView;
    SkillSystem m_Skills;
    MageProjectiles m_MageProjectiles;
    MageUltimate m_MageUltimate;
    KnightSkills m_KnightSkills;
    MapView m_MapView;
    ExpeditionView m_ExpeditionView;
    SettingsMenu m_SettingsMenu;
    TradeSystem m_Trade;
    GameUiControls m_UiControls;
    bool m_UiResume = false;
    bool m_WorldInteractionHeld = false;
    bool m_MouseInteractionStarted = false;
    WorldPosition m_WorldInteractionTarget{};
    std::string m_SaveMessage;
    bool m_JournalOpen = false;
    bool m_CompletionShown = false;
    InteractionBubbleView m_InteractionBubbleView;
    bool m_MapGenerated = false;
    std::string m_PreviewChapter;
};
