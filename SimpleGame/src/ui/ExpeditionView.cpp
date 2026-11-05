#include "stdafx.h"
#include "ExpeditionView.h"
#include <algorithm>
#include <cmath>
#include "SceneCamera.h"
#include "HudTextLayout.h"
#include "GameUiTheme.h"

namespace
{
    const Color Gold(.95f, .78f, .39f);
    const Color Ink(.025f, .04f, .05f, .97f);
    const Color Text(.77f, .83f, .79f);

    void Line(Renderer& r, Point a, Point b, Color color)
    {
        const float dx = b.x - a.x, dy = b.y - a.y;
        const float length = std::sqrt(dx * dx + dy * dy);
        if (length < .01f)
        {
            return;
        }
        const float nx = -dy / length, ny = dx / length;
        r.Quad({a.x + nx, a.y + ny},
               {b.x + nx, b.y + ny},
               {b.x - nx, b.y - ny},
               {a.x - nx, a.y - ny},
               color);
    }
}

void ExpeditionView::DrawGuide(Renderer& r,
                               const GameSession& session,
                               const LevelMap& map,
                               WorldPosition player,
                               int width) const
{
    const auto marker = session.TrackedGoal();
    if (!marker)
    {
        const float w = (std::min)(360.f, width - 32.f);
        const float x = width - w - 16.f;
        const float y = width < 760 ? 148.f : 16.f;
        GameUiTheme::Panel(r, x, y, w, 86);
        r.Text(x + 16, y + 27, "여정 기록", GameUiTheme::Gold());
        r.Text(x + 16,
               y + 56,
               HudTextLayout::Fit(r, session.Objective(), w - 32),
               GameUiTheme::Text());
        return;
    }
    const float dx = marker->position.x - player.x;
    const float dy = marker->position.y - player.y;
    const float distance = std::sqrt(dx * dx + dy * dy);
    const float x = (std::max)(16.f, float(width) - 376.f);
    const float panelWidth = (std::min)(360.f, float(width) - 32.f);
    const float y = width < 760 ? 148.f : 16.f;
    GameUiTheme::Panel(r, x, y, panelWidth, 124);
    r.Text(x + 16, y + 25, "진행 중인 임무", GameUiTheme::Gold());
    r.Text(x + panelWidth - 70, y + 25, "[M] 지도", GameUiTheme::Muted());
    r.Rect(x + 15, y + 36, panelWidth - 30, 1, Color(.25f, .27f, .24f));
    const float projectedY = dy * SceneCamera::GroundDepthScale();
    const float scale = 13.f / (std::max)(1.f, std::hypot(dx, projectedY));
    const Point center{x + 30, y + 66};
    const Point tip{center.x + dx * scale, center.y + projectedY * scale};
    GameUiTheme::Diamond(r, center.x, center.y, 19, Color(.16f, .20f, .21f));
    Line(r, center, tip, Gold);
    GameUiTheme::Diamond(r, tip.x, tip.y, 3, Gold);
    r.Text(
        x + 60, y + 61, HudTextLayout::Fit(r, marker->label, panelWidth - 76), GameUiTheme::Text());
    const auto* nearby = map.Layout().NearbyLandmark(player);
    const bool targetSelected = nearby && std::hypot(nearby->position.x - marker->position.x,
                                                     nearby->position.y - marker->position.y) < 1.f;
    const std::string instruction =
        targetSelected || distance < 90.f
            ? marker->hint.empty() ? "[Space] 주변 단서 확인" : marker->hint
            : "목표까지 " + std::to_string(static_cast<int>(std::ceil(distance / 10.f))) +
                  " · 화살표 방향으로 이동";
    r.Text(
        x + 60, y + 84, HudTextLayout::Fit(r, instruction, panelWidth - 76), GameUiTheme::Gold());
    r.Text(x + 16,
           y + 110,
           HudTextLayout::Fit(r, session.StoryChapter().Title(), panelWidth - 32),
           GameUiTheme::Muted());
}

void ExpeditionView::DrawMap(Renderer& r,
                             const GameSession& session,
                             const LevelMap& map,
                             WorldPosition player,
                             int width,
                             int height) const
{
    const float w = (std::min)(1100.f, float(width - 32));
    const float h = (std::min)(580.f, float(height - 150));
    if (w < 560.f || h < 350.f)
    {
        r.Text(20, 155, "지도를 보려면 창을 넓혀 주세요. [M] 닫기", Gold);
        return;
    }
    const float x = (width - w) * .5f, y = 128.f;
    r.Rect(0, 0, float(width), float(height), Color(0, 0, 0, .45f));
    GameUiTheme::Panel(r, x, y, w, h);
    r.Text(x + 22, y + 28, "변경과 북부 전선  /  세계 지도 [M]", Gold);
    float minX = (std::min)(-1300.f, player.x - 300.f);
    float maxX = (std::max)(2400.f, player.x + 300.f);
    float minY = player.y - 300.f, maxY = player.y + 300.f;
    for (const auto& road : map.Layout().Roads())
    {
        for (const auto p : {road.from, road.to})
        {
            minX = (std::min)(minX, p.x - 200.f);
            maxX = (std::max)(maxX, p.x + 200.f);
            minY = (std::min)(minY, p.y - 400.f);
            maxY = (std::max)(maxY, p.y + 400.f);
        }
    }
    const float scale = (std::min)((w - 415.f) / (maxX - minX), (h - 100.f) / (maxY - minY));
    const float mapX = x + 360.f + (w - 400.f - (maxX - minX) * scale) * .5f;
    const auto project = [&](WorldPosition p)
    {
        return Point{mapX + (p.x - minX) * scale, y + 50.f + (p.y - minY) * scale};
    };
    for (const auto& road : map.Layout().Roads())
    {
        Line(r, project(road.from), project(road.to), Color(.36f, .42f, .34f));
    }
    const auto& atlas = session.Atlas();
    const float rowHeight =
        (std::min)(34.f,
                   (h - 175.f) /
                       static_cast<float>((std::max)(std::size_t{1}, atlas.Regions().size())));
    for (std::size_t i = 0; i < atlas.Regions().size(); ++i)
    {
        const auto& region = atlas.Regions()[i];
        const auto p = project(atlas.ToWorld(i, region.local.entry));
        const auto color = region.discovered ? Text : Color(.39f, .44f, .44f);
        r.Ellipse(p.x, p.y, 5, 5, color);
        r.Text(p.x + 10, p.y + 4, std::to_string(i + 1), color);
        r.Text(x + 22,
               y + 66 + float(i) * rowHeight,
               HudTextLayout::Fit(r,
                                  std::to_string(i + 1) + ". " + region.local.title +
                                      (region.local.portalInterior ? " [포탈 실내]" : "") +
                                      (region.discovered ? " ✓" : ""),
                                  310.f),
               color);
    }
    if (const auto target = session.TrackedGoal())
    {
        const auto p = project(target->position);
        r.Ellipse(p.x, p.y, 8, 8, Color(.95f, .78f, .39f, .35f));
        r.Ellipse(p.x, p.y, 3, 3, Gold);
        r.Text(x + 22, y + h - 90, "목적지: " + target->label, Gold);
    }
    const auto camp = project(session.Checkpoint());
    r.Rect(camp.x - 4, camp.y - 4, 8, 8, Color(.36f, .8f, .9f));
    const auto p = project(player);
    r.Ellipse(p.x, p.y, 4, 4, Color(1.f, 1.f, 1.f));
    r.Text(x + 22, y + h - 62, "흰 점: 현재 위치 / 금빛: 목표 / 파랑: 복귀 야영지", Text);
    r.Text(x + 22, y + h - 37, "야외는 도보 이동 · 기록관 실내는 [Space] 포탈 · [J] 기록", Text);
    r.Text(x + 22, y + h - 15, "먼 지역의 전투는 대기하며, 재방문하면 이어집니다.", Text);
}

void ExpeditionView::DrawJournal(Renderer& r,
                                 const GameSession& session,
                                 int width,
                                 int height) const
{
    const float w = (std::min)(1000.f, float(width - 40));
    const float h = (std::min)(500.f, float(height - 160));
    if (w < 900.f || h < 400.f)
    {
        r.Text(20, 155, "여정 기록을 보려면 창을 넓혀 주세요. [J] 닫기", Gold);
        return;
    }
    const float x = (width - w) * .5f, y = 130.f;
    r.Rect(0, 0, float(width), float(height), Color(0, 0, 0, .45f));
    GameUiTheme::Panel(r, x, y, w, h);
    r.Text(x + 24, y + 32, "여정 기록 [J]  /  탐험과 작전", Gold);
    r.Text(x + 24, y + 69, session.StoryChapter().Title(), Text);
    r.Text(x + 24, y + 99, HudTextLayout::Fit(r, session.Objective(), w - 48.f), Gold);
    r.Text(x + 24, y + 140, "마을의 습격 조사 → 수송대 추적과 구출 → 폐광 군단 저지", Text);
    r.Text(x + 24, y + 168, "회색벽 정찰 → 협곡의 세 위기 대응 → 바르칸과 공동 전투", Text);
    const auto* third = session.FindChapter("chapter3");
    if (third && &session.StoryChapter() == third)
    {
        const auto flags = third->ExportFlags();
        const auto value = [&](const char* key)
        {
            const auto found = flags.find(key);
            return found == flags.end() ? 0 : found->second;
        };
        r.Text(x + 24, y + 202, "기념홀 → 세 독립 증거 → 봉인 서고 → 원본 보존 → 왕실 심의", Gold);
        r.Text(
            x + 24,
            y + 232,
            "확인된 사실: " +
                std::string(value("C03_SEVENTH_EXISTED") ? "실존·반복 삭제·사상 변화" : "조사 중") +
                (value("C03_PREWAR_EXPEDITION") ? " / 전쟁 전 인간군 원정" : ""),
            Text);
        r.Text(x + 24,
               y + 262,
               "미확인: 일곱째 이름 · 최초 삭제 명령자 · 원정 목적과 결과 · 전쟁의 발단",
               Text);
        r.Text(x + 24,
               y + 292,
               "필수 보관처 " + std::to_string(value("C03_DISTRIBUTED_COPIES")) +
                   "/3 · 추가 보관처 " +
                   (value("C03_EXTRA_CUSTODIAN") == 1   ? "성직자"
                    : value("C03_EXTRA_CUSTODIAN") == 2 ? "상인 조합"
                                                        : "미선택") +
                   " · 세라드 " + (value("C03_SERAD_SURVIVED") ? "생존 제압" : "미해결"),
               Text);
        r.Text(x + 24,
               y + 322,
               value("C03_ROYAL_REVIEW")
                   ? session.Complete()
                         ? "제한 조사 승인 · 왕실 심의 중 · 용사 지휘권 유지 · 다음: 야수족 영역"
                         : "왕실 심의 중 · 지원 제한 · 용사 공식 지휘권 유지 · 광역 수배 아님"
                   : "교전: Ctrl/스킬로 기력 소진 · 공격 직전 Space 패링 · 소진 후 Space 유지",
               Gold);
    }
    const auto* chapter = session.FindChapter("chapter2");
    if (chapter && (!third || &session.StoryChapter() != third))
    {
        const auto flags = chapter->ExportFlags();
        const auto value = [&](const char* key)
        {
            const auto found = flags.find(key);
            return found == flags.end() ? 0 : found->second;
        };
        r.Text(x + 24, y + 216, "협곡의 선택 · 먼저 도착한 목표부터 대응합니다.", Gold);
        r.Text(x + 24,
               y + 246,
               "봉화탑: 늦을수록 정예 증원 / 작업장: 교량·보급 손실 / 수용소: 민간 피해",
               Text);
        r.Text(x + 24,
               y + 282,
               "증원 " + std::to_string(value("C02_ELITE_REINFORCEMENT")) + " · 교량 피해 " +
                   std::to_string(value("C02_BRIDGE_DAMAGE")) + " · 민간 피해 " +
                   std::to_string(value("C02_REFUGEE_LOSS")),
               Text);
        if (session.Complete())
        {
            r.Text(x + 24,
                   y + 324,
                   "작전 완료 · 생존자와 벽화의 기록을 남겼습니다. 세계를 계속 탐험할 수 있습니다.",
                   Gold);
        }
    }
    r.Text(x + 24,
           y + h - 44,
           "야영지 [Space]: 휴식과 복귀 지점 등록 · 전투 밖 [B]: 마지막 야영지로 복귀",
           Text);
    r.Text(
        x + 24,
        y + h - 18,
        "Esc → 여정 저장 / 불러오기 · F5 저장 · 시작 화면 Enter 이어하기. 수동 저장 이후 진행은 종료 시 사라집니다.",
        Text);
}

void ExpeditionView::DrawDefeat(Renderer& r, int width, int height) const
{
    r.Rect(0, 0, float(width), float(height), Color(.01f, .02f, .025f, .76f));
    const float x = width * .5f - 275.f, y = height * .5f - 105.f;
    r.Rect(x, y, 550, 210, Ink);
    r.Text(x + 35, y + 44, "쓰러졌습니다", Gold);
    r.Text(x + 35, y + 85, "조사한 단서와 완료한 임무, 레벨과 장비는 남습니다.", Text);
    r.Text(x + 35, y + 120, "진행 중이던 전투를 다시 준비합니다.", Text);
    r.Text(x + 35, y + 172, "[Enter] 마지막 야영지에서 다시 시작", Gold);
}

void ExpeditionView::DrawPause(Renderer& r, int width, int height, bool focused) const
{
    r.Rect(0, 0, float(width), float(height), Color(.01f, .02f, .025f, .58f));
    const float panelWidth = (std::max)(1.f, (std::min)(520.f, float(width) - 32.f));
    const float x = (width - panelWidth) * .5f;
    const float y = height * .5f - 60.f;
    GameUiTheme::Panel(r, x, y, panelWidth, 120);
    r.Rect(x, y, panelWidth, 2, Gold);
    const auto line = [&](float baseline, const std::string& text, Color color)
    {
        const auto fitted = HudTextLayout::Fit(r, text, panelWidth - 32.f);
        r.Text(x + (panelWidth - r.TextWidth(fitted)) * .5f, baseline, fitted, color);
    };
    line(y + 37, focused ? "일시정지" : "다른 창을 사용 중입니다", Gold);
    line(y + 69,
         focused ? "[P] 계속하기 · [Esc] 시스템 메뉴" : "게임 창을 선택하면 돌아갑니다",
         Text);
    line(y + 98, "이동·공격과 게임 진행이 멈춰 있습니다.", Color(.59f, .66f, .64f));
}
