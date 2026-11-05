#pragma once
#include "Renderer.h"
#include <string>
#include <vector>

class GameUiControls
{
  public:
    enum class Window
    {
        None,
        Inventory,
        Skills,
        Map,
        Journal,
        Trade,
        Menu
    };
    enum class Action
    {
        None,
        Inventory,
        Skills,
        Map,
        Journal,
        Trade,
        Menu,
        Health,
        Mana,
        Save,
        Interact,
        Attack,
        Camp,
        Close,
        Previous,
        Next,
        Retry,
        Resume,
        Quick0 = 40,
        Quick1,
        Quick2,
        Quick3,
        Quick4,
        Quick5,
        Quick6,
        Quick7
    };

    struct State
    {
        Window window = Window::None;
        bool paused = false;
        bool defeated = false;
        int healthPotions = 0;
        int manaPotions = 0;
        int skillCount = 6;
    };

    struct Rect
    {
        float x, y, width, height;
        bool Contains(int px, int py) const;
    };

    struct Button
    {
        Action action;
        Rect rect;
        std::string label;
        bool enabled = true;
        bool painted = true;
    };

    static std::vector<Button> Layout(int width, int height, const State& state);
    void Draw(Renderer& renderer, int width, int height, const State& state) const;
    bool Contains(int x, int y, int width, int height, const State& state) const;
    Action MouseButton(bool down, int x, int y, int width, int height, const State& state);
    void MouseMove(int x, int y, int width, int height, const State& state);
    bool HoldingInteract() const;
    bool HasPress() const;
    void Cancel();

  private:
    Action Hit(int x, int y, int width, int height, const State& state) const;
    const std::vector<Button>& Buttons(int width, int height, const State& state) const;
    Action m_Pressed = Action::None;
    Action m_Hovered = Action::None;
    mutable std::vector<Button> m_CachedButtons;
    mutable State m_CachedState;
    mutable int m_CachedWidth = -1, m_CachedHeight = -1;
};
