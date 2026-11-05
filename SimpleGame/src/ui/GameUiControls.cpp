#include "stdafx.h"
#include "GameUiControls.h"
#include "GameUiTheme.h"
#include "SkillLayout.h"
#include <algorithm>

bool GameUiControls::Rect::Contains(int px, int py) const
{
    return px >= x && px < x + width && py >= y && py < y + height;
}

std::vector<GameUiControls::Button> GameUiControls::Layout(int width, int height, const State& s)
{
    std::vector<Button> buttons;
    buttons.reserve(24);
    const auto add =
        [&](Action a, Rect box, const std::string& text, bool enabled = true, bool painted = true)
    {
        buttons.push_back({a, box, text, enabled, painted});
    };
    if (width < 360 || height < 300)
    {
        const auto action = s.defeated                 ? Action::Retry
                            : s.window == Window::None ? Action::Menu
                                                       : Action::Close;
        add(action,
            {(std::max)(2.f, width - 74.f),
             (std::max)(2.f, height - 36.f),
             (std::max)(1.f, (std::min)(70.f, width - 4.f)),
             28},
            s.defeated                 ? "재도전"
            : s.window == Window::None ? "메뉴"
                                       : "닫기");
        return buttons;
    }
    const bool playing = !s.paused && !s.defeated && s.window == Window::None;
    if (s.window == Window::None)
    {
        const int columns = width >= 760 ? 6 : 4;
        const int rows = 12 / columns;
        const float left = (std::max)(140.f, width * .43f);
        const float buttonWidth = (std::max)(12.f, (width - left - 14.f) / columns - 4.f);
        const float y = height - 106.f;
        const float rowHeight = rows == 2 ? 37.f : 25.f;
        const Action actions[] = {Action::Inventory,
                                  Action::Skills,
                                  Action::Map,
                                  Action::Journal,
                                  Action::Trade,
                                  Action::Menu,
                                  Action::Health,
                                  Action::Mana,
                                  Action::Save,
                                  Action::Interact,
                                  Action::Attack,
                                  Action::Camp};
        const std::string labels[] = {"가방",
                                      "스킬",
                                      "지도",
                                      "기록",
                                      "상점",
                                      "메뉴",
                                      "HP " + std::to_string(s.healthPotions),
                                      "MP " + std::to_string(s.manaPotions),
                                      "저장",
                                      "확인·유지",
                                      "공격",
                                      "귀환"};
        for (int i = 0; i < 12; ++i)
        {
            const bool gameplay = i == 6 || i == 7 || i >= 9;
            add(actions[i],
                {left + (i % columns) * (buttonWidth + 4.f),
                 y + (i / columns) * (rowHeight + 3.f),
                 buttonWidth,
                 rowHeight},
                labels[i],
                !s.defeated && (!gameplay || playing) && !(s.paused && (i == 1 || i == 4)));
        }
    }
    if (playing)
    {
        const auto layout = SkillLayout::Create(width, height, s.skillCount);
        for (int i = 0; i < 8; ++i)
        {
            add(static_cast<Action>(static_cast<int>(Action::Quick0) + i),
                {layout.SlotX(i), layout.slotsY, layout.slotSize, layout.slotSize},
                "",
                true,
                false);
        }
    }
    Rect panel{};
    bool close = false;
    if (s.window == Window::Inventory && width >= 608 && height >= 452)
    {
        panel = {(width - 584.f) * .5f, (height - 404.f) * .5f, 584, 404};
        close = true;
        add(Action::Previous, {panel.x + 500, panel.y + 75, 28, 24}, "<");
        add(Action::Next, {panel.x + 536, panel.y + 75, 28, 24}, ">");
    }
    else if (s.window == Window::Trade && width >= 584 && height >= 440)
    {
        panel = {(width - 560.f) * .5f, (height - 400.f) * .5f, 560, 400};
        close = true;
    }
    else if (s.window == Window::Skills)
    {
        const auto l = SkillLayout::Create(width, height, s.skillCount);
        if (l.canShowWindow)
        {
            panel = {l.panelX, l.panelY, l.panelWidth, 350};
            close = true;
        }
    }
    else if (s.window == Window::Map)
    {
        const float w = (std::min)(1100.f, width - 32.f), h = (std::min)(580.f, height - 150.f);
        if (w >= 560 && h >= 350)
        {
            panel = {(width - w) * .5f, 128, w, h};
            close = true;
        }
    }
    else if (s.window == Window::Journal)
    {
        const float w = (std::min)(1000.f, width - 40.f), h = (std::min)(500.f, height - 160.f);
        if (w >= 900 && h >= 400)
        {
            panel = {(width - w) * .5f, 130, w, h};
            close = true;
        }
    }
    else if (s.window == Window::Menu)
    {
        const float w = (std::min)(560.f, (std::max)(1.f, width - 24.f));
        const float h = (std::min)(590.f, (std::max)(1.f, height - 24.f));
        panel = {(width - w) * .5f, (height - h) * .5f, w, h};
        close = true;
    }
    if (close)
    {
        add(Action::Close, {panel.x + panel.width - 28, panel.y + 6, 20, 20}, "×");
    }
    else if (s.window != Window::None)
    {
        add(Action::Close, {width - 76.f, 12, 64, 26}, "닫기");
    }
    if (s.defeated)
    {
        add(Action::Retry, {width * .5f - 90, height * .5f + 43, 180, 30}, "다시 도전");
    }
    else if (s.paused && s.window == Window::None)
    {
        add(Action::Resume, {width * .5f - 72, height * .5f + 28, 144, 26}, "게임 계속");
    }
    return buttons;
}

void GameUiControls::Draw(Renderer& r, int width, int height, const State& state) const
{
    for (const auto& button : Buttons(width, height, state))
    {
        const auto& b = button.rect;
        if (button.painted)
        {
            GameUiTheme::Button(r,
                                b.x,
                                b.y,
                                b.width,
                                b.height,
                                button.label,
                                m_Hovered == button.action,
                                m_Pressed == button.action,
                                button.enabled);
            if ((button.action == Action::Health || button.action == Action::Mana) && b.width >= 70)
            {
                const float x = b.x + 10, y = b.y + b.height * .5f;
                const Color liquid = button.action == Action::Health ? Color(.52f, .14f, .19f)
                                                                     : Color(.13f, .25f, .46f);
                r.Rect(x - 3, y - 8, 6, 4, Color(.32f, .22f, .14f));
                r.Rect(x - 5, y - 4, 10, 12, Color(.32f, .40f, .46f));
                r.Rect(x - 4, y - 1, 8, 8, button.enabled ? liquid : GameUiTheme::Muted());
                r.Rect(x - 2, y - 3, 2, 6, Color(.65f, .72f, .75f, .40f));
            }
        }
        else if (m_Hovered == button.action)
        {
            r.Rect(b.x, b.y, b.width, 2, GameUiTheme::Gold());
            r.Rect(b.x, b.y + b.height - 2, b.width, 2, GameUiTheme::Gold());
        }
    }
}

GameUiControls::Action GameUiControls::Hit(
    int x, int y, int width, int height, const State& state) const
{
    const auto& buttons = Buttons(width, height, state);
    for (auto i = buttons.rbegin(); i != buttons.rend(); ++i)
    {
        if (i->enabled && i->rect.Contains(x, y))
        {
            return i->action;
        }
    }
    return Action::None;
}

bool GameUiControls::Contains(int x, int y, int width, int height, const State& state) const
{
    for (const auto& b : Buttons(width, height, state))
    {
        if (b.rect.Contains(x, y))
        {
            return true;
        }
    }
    return false;
}

GameUiControls::Action GameUiControls::MouseButton(
    bool down, int x, int y, int width, int height, const State& state)
{
    m_Hovered = Hit(x, y, width, height, state);
    if (down)
    {
        m_Pressed = m_Hovered;
        return Action::None;
    }
    const auto pressed = m_Pressed;
    m_Pressed = Action::None;
    return pressed == m_Hovered ? m_Hovered : Action::None;
}

void GameUiControls::MouseMove(int x, int y, int width, int height, const State& state)
{
    m_Hovered = Hit(x, y, width, height, state);
}

bool GameUiControls::HoldingInteract() const
{
    return m_Pressed == Action::Interact && m_Hovered == Action::Interact;
}

bool GameUiControls::HasPress() const
{
    return m_Pressed != Action::None;
}

void GameUiControls::Cancel()
{
    m_Pressed = m_Hovered = Action::None;
}

const std::vector<GameUiControls::Button>& GameUiControls::Buttons(int width,
                                                                   int height,
                                                                   const State& s) const
{
    const auto& p = m_CachedState;
    if (width != m_CachedWidth || height != m_CachedHeight || s.window != p.window ||
        s.paused != p.paused || s.defeated != p.defeated || s.healthPotions != p.healthPotions ||
        s.manaPotions != p.manaPotions || s.skillCount != p.skillCount)
    {
        m_CachedButtons = Layout(width, height, s);
        m_CachedState = s;
        m_CachedWidth = width;
        m_CachedHeight = height;
    }
    return m_CachedButtons;
}
