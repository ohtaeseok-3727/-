#include "stdafx.h"
#include "TitleScreenView.h"
#include "GameUiTheme.h"

namespace
{
    struct TitleLayout
    {
        float center, heading, cardWidth, top, cardHeight;

        float CardX(int profession) const
        {
            return center - 12.f + (profession == 0 ? -cardWidth : 24.f);
        }
    };

    TitleLayout Layout(int width, int height)
    {
        const float heading = height < 620 ? 62.f : 96.f;
        const float top = heading + 80.f;
        return {width * .5f,
                heading,
                (std::min)(310.f, (width - 88.f) * .5f),
                top,
                (std::min)(350.f, height - top - 68.f)};
    }
}

void TitleScreenView::Draw(Renderer& r, int width, int height) const
{
    const auto layout = Layout(width, height);
    const float center = layout.center;
    r.Rect(0, 0, float(width), float(height), Color(.008f, .018f, .025f, .74f));
    r.Rect(0, 0, float(width), 6, GameUiTheme::Gold());
    const float heading = layout.heading;
    GameUiTheme::Diamond(r, center - 160, heading - 16, 7, GameUiTheme::Gold());
    GameUiTheme::Diamond(r, center + 160, heading - 16, 7, GameUiTheme::Gold());
    GameUiTheme::Center(r, center, heading, "긴 전쟁", GameUiTheme::Text(), 2.8f);
    GameUiTheme::Center(r, center, heading + 29, "변경의 여정", GameUiTheme::Gold(), 1.15f);
    GameUiTheme::Center(r, center, heading + 55, "당신의 전투를 선택하세요", GameUiTheme::Muted());
    const float cardWidth = layout.cardWidth;
    const float top = layout.top;
    const float cardHeight = layout.cardHeight;
    for (int profession = 0; profession < 2; ++profession)
    {
        const float x = layout.CardX(profession);
        const Color accent = profession == 0 ? Color(.33f, .64f, .74f) : Color(.82f, .59f, .33f);
        GameUiTheme::Panel(r, x, top, cardWidth, cardHeight);
        if (m_Hovered == profession)
        {
            r.Rect(x + 3,
                   top + 3,
                   cardWidth - 6,
                   cardHeight - 6,
                   Color(accent.r, accent.g, accent.b, m_Pressed == profession ? .16f : .08f));
        }
        r.Rect(x + 1, top + 1, cardWidth - 2, 3, accent);
        GameUiTheme::Center(r,
                            x + cardWidth * .5f,
                            top + 39,
                            profession == 0 ? "마법사" : "기사",
                            GameUiTheme::Text(),
                            1.45f);
        r.Ellipse(x + cardWidth * .5f,
                  top + cardHeight * .60f,
                  60,
                  16,
                  Color(accent.r, accent.g, accent.b, .10f));
        r.Character(profession, x + cardWidth * .5f, top + cardHeight * .60f, false, 2);
        GameUiTheme::Center(r,
                            x + cardWidth * .5f,
                            top + cardHeight - 86,
                            profession == 0 ? "룬 장갑 · 마법과 근접 전투"
                                            : "검과 방패 · 연속 공격",
                            accent);
        GameUiTheme::Center(r,
                            x + cardWidth * .5f,
                            top + cardHeight - 58,
                            profession == 0 ? "마력을 엮어 전장을 지배합니다"
                                            : "전열을 지키고 적을 돌파합니다",
                            GameUiTheme::Muted());
        r.Rect(x + 20, top + cardHeight - 40, cardWidth - 40, 25, Color(.12f, .16f, .17f));
        GameUiTheme::Center(r,
                            x + cardWidth * .5f,
                            top + cardHeight - 22,
                            profession == 0 ? "[1] 또는 클릭하여 시작" : "[2] 또는 클릭하여 시작",
                            GameUiTheme::Text());
    }
    GameUiTheme::Center(
        r,
        center,
        height - 20.f,
        HudTextLayout::Fit(
            r, "방향키 이동  ·  Ctrl 공격  ·  Space 상호작용  ·  F10 종료", width - 32.f),
        GameUiTheme::Muted());
}

void TitleScreenView::MouseMove(int x, int y, int width, int height)
{
    const auto layout = Layout(width, height);
    m_Hovered = -1;
    for (int profession = 0; profession < 2; ++profession)
    {
        const float left = layout.CardX(profession);
        if (x >= left && x < left + layout.cardWidth && y >= layout.top &&
            y < layout.top + layout.cardHeight)
        {
            m_Hovered = profession;
        }
    }
}

int TitleScreenView::MouseButton(bool down, int x, int y, int width, int height)
{
    MouseMove(x, y, width, height);
    if (down)
    {
        m_Pressed = m_Hovered;
        return -1;
    }
    const int selected = m_Pressed == m_Hovered ? m_Hovered : -1;
    CancelPress();
    return selected;
}

void TitleScreenView::CancelPress()
{
    m_Pressed = -1;
}
