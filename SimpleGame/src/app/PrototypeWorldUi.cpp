#include "stdafx.h"
#include "PrototypeWorld.h"
#include <utility>

GameUiControls::State PrototypeWorld::UiState() const
{
    GameUiControls::State state;
    using W = GameUiControls::Window;
    state.window = m_SettingsMenu.IsOpen()       ? W::Menu
                   : m_Trade.IsOpen()            ? W::Trade
                   : m_LevelView.InventoryOpen() ? W::Inventory
                   : m_Skills.IsWindowOpen()     ? W::Skills
                   : m_MapView.IsOpen()          ? W::Map
                   : m_JournalOpen               ? W::Journal
                                                 : W::None;
    state.paused = m_Paused;
    state.defeated = m_Started && m_Level.IsDefeated();
    state.healthPotions = m_Level.Inventory().HealthPotions();
    state.manaPotions = m_Level.Inventory().ManaPotions();
    state.skillCount = static_cast<int>(m_Skills.PlayerSkillsState().Available().size());
    return state;
}

bool PrototypeWorld::ConsumeUiResume()
{
    return std::exchange(m_UiResume, false);
}

void PrototypeWorld::ApplyUiAction(GameUiControls::Action action)
{
    using A = GameUiControls::Action;
    switch (action)
    {
    case A::Inventory:
        ToggleInventory();
        break;
    case A::Skills:
        ToggleSkills();
        break;
    case A::Map:
        ToggleMap();
        break;
    case A::Journal:
        ToggleJournal();
        break;
    case A::Trade:
        ToggleTrade();
        break;
    case A::Menu:
        m_Paused = false;
        m_UiResume = true;
        ToggleSystemMenu();
        break;
    case A::Health:
        UsePotion(false);
        break;
    case A::Mana:
        UsePotion(true);
        break;
    case A::Save:
        SaveGame();
        break;
    case A::Interact:
        if (!m_MouseInteractionStarted)
        {
            InspectLandmark();
        }
        m_MouseInteractionStarted = false;
        break;
    case A::Camp:
        ReturnToCamp();
        break;
    case A::Close:
        CloseMenus();
        break;
    case A::Previous:
        ChangeInventoryPage(-1);
        break;
    case A::Next:
        ChangeInventoryPage(1);
        break;
    case A::Retry:
        RetryDefeat();
        break;
    case A::Resume:
        m_Paused = false;
        m_UiResume = true;
        break;
    case A::Attack:
        if (!m_Paused && !MenuOpen() && !m_Level.IsDefeated())
        {
            UpdatePlayerState();
            if (m_PlayerState.CanAttack())
            {
                m_Attack.Press(m_Class);
            }
        }
        break;
    default:
        if (action >= A::Quick0 && action <= A::Quick7 && !m_Paused && !MenuOpen() &&
            !m_Level.IsDefeated())
        {
            constexpr char keys[] = {'a', 's', 'd', 'f', 'q', 'w', 'e', 'r'};
            HandleSkillKey(keys[static_cast<int>(action) - static_cast<int>(A::Quick0)]);
        }
        break;
    }
}

bool PrototypeWorld::UiMouseButton(bool down, int x, int y, int width, int height)
{
    if (!m_Started)
    {
        return false;
    }
    if (down)
    {
        m_MouseInteractionStarted = false;
    }
    const auto state = UiState();
    const bool captured = m_UiControls.HasPress();
    const bool over = m_UiControls.Contains(x, y, width, height, state);
    const auto action = m_UiControls.MouseButton(down, x, y, width, height, state);
    if (over || captured || action != GameUiControls::Action::None)
    {
        m_WorldInteractionHeld = false;
        if (down && m_UiControls.HoldingInteract())
        {
            m_MouseInteractionStarted = true;
            InspectLandmark();
        }
        else
        {
            ApplyUiAction(action);
        }
        return true;
    }
    if (!m_Paused && !MenuOpen() && !m_Level.IsDefeated())
    {
        const auto* site = m_Map.Layout().NearbyLandmark({m_X, m_Y});
        const auto p = site ? Project(site->position.x, site->position.y) : Point{-1000, -1000};
        const bool hit = site && x >= p.x - 30 && x <= p.x + 30 && y >= p.y - 70 && y <= p.y + 16;
        if (down)
        {
            m_WorldInteractionHeld = hit;
            if (hit)
            {
                m_WorldInteractionTarget = site->position;
                m_MouseInteractionStarted = true;
                InspectLandmark();
            }
            return hit;
        }
        const bool held = std::exchange(m_WorldInteractionHeld, false);
        if (held && hit && site->position.x == m_WorldInteractionTarget.x &&
            site->position.y == m_WorldInteractionTarget.y)
        {
            if (!m_MouseInteractionStarted)
            {
                InspectLandmark();
            }
            m_MouseInteractionStarted = false;
            return true;
        }
    }
    if (!down)
    {
        m_WorldInteractionHeld = false;
        m_MouseInteractionStarted = false;
    }
    return false;
}
