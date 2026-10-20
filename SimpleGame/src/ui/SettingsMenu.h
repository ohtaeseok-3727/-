#pragma once

#include "Renderer.h"
#include <optional>
#include <vector>

// Screen-space buttons own their hit areas and modal navigation.
class SettingsMenu
{
  public:
    enum class Action
    {
        None,
        Resume,
        Settings,
        Back,
        Quit,
        SkipChapter1,
        SkipChapter2,
        SkipChapter2Story,
        SkipChapter3,
        PostProcessing,
        Bloom,
        Vignette,
        EdgeBlur,
        Darker,
        Brighter,
        Defaults,
        Confirm
    };

    bool IsOpen() const;
    void Open();
    void Close();
    void Escape();
    void CancelPress();
    void MouseMove(int x, int y);
    Action MouseButton(bool down,
                       int x,
                       int y,
                       int width,
                       int height,
                       PostSettings& settings,
                       bool skipChapter1,
                       bool skipChapter2,
                       bool skipChapter2Story,
                       bool skipChapter3);
    void Draw(Renderer& renderer,
              int width,
              int height,
              bool skipChapter1,
              bool skipChapter2,
              bool skipChapter2Story,
              bool skipChapter3) const;

  private:
    enum class Page
    {
        Main,
        Graphics,
        Confirmation
    };

    struct Button
    {
        Action action;
        std::string text;
        float x, y, width, height;
        bool enabled = true;
        bool Contains(int px, int py) const;
    };

    std::vector<Button> Buttons(int width,
                                int height,
                                const PostSettings& settings,
                                bool skipChapter1,
                                bool skipChapter2,
                                bool skipChapter2Story,
                                bool skipChapter3) const;
    Action Activate(Action action, PostSettings& settings);
    bool m_Open = false;
    Page m_Page = Page::Main;
    Action m_Pending = Action::None;
    std::optional<Action> m_Pressed;
    int m_MouseX = -1, m_MouseY = -1;
};
