#include "stdafx.h"
#include "SettingsMenu.h"
#include <algorithm>
#include <cmath>
#include <utility>

namespace
{
    struct Panel
    {
        float x, y, width, height;
    };

    Panel Layout(int width, int height)
    {
        const float w = (std::min)(560.f, (std::max)(1.f, width - 24.f));
        const float h = (std::min)(590.f, (std::max)(1.f, height - 24.f));
        return {(width - w) * .5f, (height - h) * .5f, w, h};
    }
}

bool SettingsMenu::IsOpen() const
{
    return m_Open;
}

void SettingsMenu::Open()
{
    m_Open = true;
    m_Page = Page::Main;
    m_Pending = Action::None;
    CancelPress();
}

void SettingsMenu::Close()
{
    m_Open = false;
    CancelPress();
}

void SettingsMenu::Escape()
{
    if (m_Page == Page::Main)
    {
        Close();
    }
    else
    {
        m_Page = Page::Main;
        m_Pending = Action::None;
        CancelPress();
    }
}

void SettingsMenu::CancelPress()
{
    m_Pressed.reset();
}

void SettingsMenu::MouseMove(int x, int y)
{
    m_MouseX = x;
    m_MouseY = y;
}

bool SettingsMenu::Button::Contains(int px, int py) const
{
    return px >= x && px < x + width && py >= y && py < y + height;
}

std::vector<SettingsMenu::Button> SettingsMenu::Buttons(
    int width, int height, const PostSettings& settings, bool skipChapter1, bool skipChapter2) const
{
    const auto panel = Layout(width, height);
    const float row = (panel.height - 150.f) / 7.f;
    std::vector<Button> buttons;
    const auto add = [&](Action action, std::string text, int index, bool enabled = true)
    {
        buttons.push_back({action,
                           std::move(text),
                           panel.x + 24.f,
                           panel.y + 94.f + row * index,
                           panel.width - 48.f,
                           row - 8.f,
                           enabled});
    };
    const auto toggle = [](const char* text, bool enabled)
    {
        return std::string(text) + (enabled ? "   켜짐" : "   꺼짐");
    };
    if (m_Page == Page::Main)
    {
        add(Action::Settings, "게임 설정", 0);
        add(Action::SkipChapter1, "챕터 1 스킵  →  회색벽 입구", 1, skipChapter1);
        add(Action::SkipChapter2, "챕터 2 필드 스토리 스킵  →  성채 입구", 2, skipChapter2);
        add(Action::Quit, "게임 종료", 4);
        add(Action::Resume, "게임으로 돌아가기", 6);
    }
    else if (m_Page == Page::Graphics)
    {
        add(Action::PostProcessing, toggle("화면 후처리", settings.enabled), 0);
        add(Action::Bloom, toggle("빛 번짐", settings.bloom), 1, settings.enabled);
        add(Action::Vignette, toggle("가장자리 어둡게", settings.vignette), 2, settings.enabled);
        add(Action::EdgeBlur, toggle("가장자리 흐림", settings.edgeBlur), 3, settings.enabled);
        add(Action::Darker, "밝기 −", 4, settings.enabled && settings.exposure > .1f);
        buttons.back().width = (panel.width - 60.f) * .5f;
        const float halfWidth = buttons.back().width;
        add(Action::Brighter, "밝기 +", 4, settings.enabled && settings.exposure < 5.f);
        buttons.back().x += halfWidth + 12.f;
        buttons.back().width = halfWidth;
        add(Action::Defaults, "기본 설정으로 복원", 5);
        add(Action::Back, "뒤로", 6);
    }
    else
    {
        add(Action::Confirm, m_Pending == Action::Quit ? "게임 종료" : "스킵 실행", 4);
        add(Action::Back, "취소", 6);
    }
    return buttons;
}

SettingsMenu::Action SettingsMenu::MouseButton(bool down,
                                               int x,
                                               int y,
                                               int width,
                                               int height,
                                               PostSettings& settings,
                                               bool skipChapter1,
                                               bool skipChapter2)
{
    MouseMove(x, y);
    if (!m_Open)
    {
        return Action::None;
    }
    std::optional<Action> hit;
    for (const auto& button : Buttons(width, height, settings, skipChapter1, skipChapter2))
    {
        if (button.enabled && button.Contains(x, y))
        {
            hit = button.action;
            break;
        }
    }
    if (down)
    {
        m_Pressed = hit;
        return Action::None;
    }
    const auto pressed = m_Pressed;
    CancelPress();
    return hit && hit == pressed ? Activate(*hit, settings) : Action::None;
}

SettingsMenu::Action SettingsMenu::Activate(Action action, PostSettings& settings)
{
    switch (action)
    {
    case Action::Resume:
        Close();
        break;
    case Action::Settings:
        m_Page = Page::Graphics;
        break;
    case Action::Back:
        m_Page = Page::Main;
        m_Pending = Action::None;
        break;
    case Action::Quit:
    case Action::SkipChapter1:
    case Action::SkipChapter2:
        m_Pending = action;
        m_Page = Page::Confirmation;
        break;
    case Action::Confirm:
    {
        const auto result = m_Pending;
        Close();
        return result;
    }
    case Action::PostProcessing:
        settings.enabled = !settings.enabled;
        break;
    case Action::Bloom:
        settings.bloom = !settings.bloom;
        break;
    case Action::Vignette:
        settings.vignette = !settings.vignette;
        break;
    case Action::EdgeBlur:
        settings.edgeBlur = !settings.edgeBlur;
        break;
    case Action::Darker:
    case Action::Brighter:
        settings.exposure =
            std::clamp(settings.exposure + (action == Action::Darker ? -.1f : .1f), .1f, 5.f);
        break;
    case Action::Defaults:
        settings = PostSettings{};
        break;
    default:
        break;
    }
    return Action::None;
}

void SettingsMenu::Draw(
    Renderer& r, int width, int height, bool skipChapter1, bool skipChapter2) const
{
    if (!m_Open)
    {
        return;
    }
    const auto panel = Layout(width, height);
    r.Rect(0, 0, float(width), float(height), Color(.01f, .015f, .025f, .7f));
    r.Rect(panel.x + 6, panel.y + 8, panel.width, panel.height, Color(0, 0, 0, .45f));
    r.Rect(panel.x, panel.y, panel.width, panel.height, Color(.045f, .06f, .08f, .98f));
    r.Rect(panel.x, panel.y, panel.width, 3, Color(.76f, .60f, .31f));
    const std::string title = m_Page == Page::Graphics       ? "게임 설정"
                              : m_Page == Page::Confirmation ? "선택 확인"
                                                             : "시스템 메뉴";
    r.Text(panel.x + (panel.width - r.TextWidth(title)) * .5f,
           panel.y + 38,
           title,
           Color(.96f, .85f, .62f));
    const std::string subtitle =
        m_Page == Page::Graphics
            ? "밝기 " + std::to_string(static_cast<int>(std::lround(r.Effects().exposure * 100))) +
                  "% · 설정은 즉시 적용됩니다"
            : "게임이 일시정지되었습니다";
    r.Text(panel.x + (panel.width - r.TextWidth(subtitle)) * .5f,
           panel.y + 66,
           subtitle,
           Color(.60f, .68f, .74f));
    if (m_Page == Page::Confirmation)
    {
        std::vector<std::string> lines;
        if (m_Pending == Action::Quit)
        {
            lines = {"게임을 종료할까요?", "현재 진행은 저장되지 않습니다."};
        }
        else if (m_Pending == Action::SkipChapter1)
        {
            lines = {"챕터 1을 완료하고 회색벽 입구로 이동합니다.",
                     "레벨과 장비는 유지되며 체력·마나를 회복합니다.",
                     "건너뛴 전투의 경험치·전리품은 지급되지 않습니다."};
        }
        else
        {
            lines = {"회색벽·협곡을 완료하고 성채 입구로 이동합니다.",
                     "기존 선택을 유지하고 진행 중인 목표부터 완료합니다.",
                     "남은 목표는 봉화탑 → 작업장 → 수용소 순서입니다.",
                     "레벨·장비 유지, 체력·마나 회복. 스킵 보상은 없습니다."};
        }
        for (std::size_t i = 0; i < lines.size(); ++i)
        {
            r.Text(panel.x + 24.f,
                   panel.y + 120.f + 30.f * static_cast<float>(i),
                   lines[i],
                   Color(.79f, .84f, .87f));
        }
    }
    for (const auto& button : Buttons(width, height, r.Effects(), skipChapter1, skipChapter2))
    {
        const bool hover = button.enabled && button.Contains(m_MouseX, m_MouseY);
        const bool pressed = hover && m_Pressed == button.action;
        const Color border = hover ? Color(.87f, .70f, .38f) : Color(.23f, .29f, .35f);
        r.Rect(button.x, button.y, button.width, button.height, border);
        r.Rect(button.x + 1,
               button.y + 1,
               button.width - 2,
               button.height - 2,
               pressed ? Color(.24f, .27f, .29f)
               : hover ? Color(.14f, .19f, .24f)
                       : Color(.075f, .10f, .13f));
        r.Text(button.x + (button.width - r.TextWidth(button.text)) * .5f,
               button.y + button.height * .5f + 5.f,
               button.text,
               button.enabled ? Color(.90f, .90f, .84f) : Color(.37f, .43f, .48f));
    }
    const std::string hint = m_Page == Page::Main
                                 ? "Esc 닫기 · 완료/선행 조건에 따라 스킵 버튼 활성화"
                                 : "Esc 뒤로 · 설정과 진행은 이번 실행에만 유지됩니다";
    r.Text(panel.x + (panel.width - r.TextWidth(hint)) * .5f,
           panel.y + panel.height - 17.f,
           hint,
           Color(.53f, .61f, .67f));
}
