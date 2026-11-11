#include "stdafx.h"
#include "Chapter3.h"
#include "Chapter3Props.h"
#include "Renderer.h"
#include "SceneGraph.h"
#include "CombatCueView.h"
#include <algorithm>
#include <cmath>

namespace
{
    const Color Paper(.85f, .79f, .59f);
    const Color Stone(.35f, .4f, .46f);
    const Color Green(.35f, .85f, .62f);

    void Person(Renderer& r, Point p, Color coat, const char* label)
    {
        r.Ellipse(p.x, p.y, 16, 7, Color(0, 0, 0, .35f));
        r.Rect(p.x - 10, p.y - 34, 20, 29, coat);
        r.Ellipse(p.x, p.y - 43, 9, 10, Color(.78f, .63f, .48f));
        r.Rect(p.x - 9, p.y - 8, 6, 9, Stone);
        r.Rect(p.x + 3, p.y - 8, 6, 9, Stone);
        r.Text(p.x - 35, p.y - 60, label, Paper);
    }
}

std::string Chapter3::InteractionPrompt(const MapLandmark& site) const
{
    using namespace Chapter3Objects;
    if (site.object == MemorialEntrance || site.object == ReadingEntrance ||
        site.object == ReadingReturn || site.object == ArchiveExit ||
        site.object == ArchiveReturn || site.object == CityReturn || site.object == CityExit)
    {
        return "[Space] 기록관 포탈 · " + site.name;
    }
    if (site.object >= FoundationWest && site.object <= FoundationEast)
    {
        return "[Space 2초 유지] " + site.name;
    }
    if (site.object == Brake)
    {
        return "[Space 2초 유지] 브레이크 · 바퀴 해제 후";
    }
    if (site.object >= RefugeRoom && site.object <= LastSermon)
    {
        return "외벽의 필체·방 배치 순서로 접근 · [Space] 안내";
    }
    return Chapter::InteractionPrompt(site);
}

bool Chapter3::DrawLandmark(Renderer& r, const MapLandmark& site, Point p, float time) const
{
    using namespace Chapter3Objects;
    const auto id = site.object;
    if (id == CityGate || id == MarketSign || id == MemorialSign || id == MonasterySign)
    {
        r.Rect(p.x - 4, p.y - 42, 8, 45, Stone);
        const float width = r.TextWidth(site.name) + 20.f;
        r.Rect(p.x - width * .5f, p.y - 65, width, 26, Color(.10f, .18f, .20f));
        r.Text(p.x - width * .5f + 10, p.y - 47, site.name, Paper);
        return true;
    }
    if (id == MemorialEntrance || id == ReadingEntrance || id == ReadingReturn ||
        id == ArchiveExit || id == ArchiveReturn || id == CityReturn || id == CityExit)
    {
        r.Ellipse(p.x, p.y, 46, 18, Color(.22f, .72f, .82f, .25f));
        r.Ellipse(p.x, p.y, 31, 11, Color(.50f, .86f, .93f, .55f));
        r.Rect(p.x - 35, p.y - 69, 7, 70, Stone);
        r.Rect(p.x + 28, p.y - 69, 7, 70, Stone);
        r.Rect(p.x - 35, p.y - 75, 70, 8, Paper);
        r.Rect(p.x - 25, p.y - 65, 50, 53, Color(.15f, .48f, .57f, .45f));
        r.Text(p.x - 58, p.y - 87, "[Space] 기록관 포탈", Paper);
        return true;
    }
    std::optional<Chapter3Prop> prop;
    bool complete = false;
    if (id >= ManuscriptEarly && id <= ManuscriptLate)
    {
        prop = Chapter3Prop::Manuscript;
        complete = m_State.manuscripts > id - ManuscriptEarly;
    }
    else if (id >= FoundationWest && id <= FoundationEast)
    {
        prop = Chapter3Prop::Foundation;
        complete = m_State.foundations.test(id - FoundationWest);
    }
    else if (id >= RefugeRoom && id <= LastSermon)
    {
        prop = Chapter3Prop::Record;
        complete = m_State.monastery > id - RefugeRoom;
    }
    else if (id >= WrongDateBox && id <= OriginalBox)
    {
        prop = Chapter3Prop::Crate;
    }
    else if (id == Brake)
    {
        prop = Chapter3Prop::Brake;
        complete = m_State.brake;
    }
    else if (id == SealedWagon)
    {
        prop = Chapter3Prop::Wagon;
    }
    else if (id == FieldPermit || id == CommandSeal || id == Testimony)
    {
        prop = Chapter3Prop::Permit;
        complete = id == FieldPermit ? Complete()
                   : id == Testimony ? m_State.testimony
                                     : m_State.hearing;
    }
    else if (id == MovementList || id == StorageDate || id == WorkshopLedger ||
             id == TransferSeal || (id >= ConfirmedNotice && id <= ExtraGuild))
    {
        prop = Chapter3Prop::Record;
    }
    if (prop)
    {
        Chapter3Props::Draw(r, *prop, p, *prop == Chapter3Prop::Wagon ? 76.f : 56.f);
        r.Text(p.x - r.TextWidth(site.name) * .5f, p.y - 76.f, site.name, Paper);
        if (complete)
        {
            r.Text(p.x - 24, p.y + 18, "확인 완료", Green);
        }
        if (id == SealedWagon && m_State.hearing)
        {
            r.Text(p.x - 27, p.y + 18, "왕실 봉인", Paper);
        }
        if (m_Hold > 0.f && m_HoldAction == id)
        {
            r.Rect(p.x - 30, p.y + 25, 60, 5, Stone);
            r.Rect(p.x - 30, p.y + 25, 60 * std::clamp(m_Hold / 2.f, 0.f, 1.f), 5, Green);
        }
        return true;
    }
    r.Rect(p.x - 23, p.y - 25, 46, 25, Stone);
    r.Rect(p.x - 19, p.y - 32, 38, 7, Paper);
    if (id == Ceremony)
    {
        for (int i = 0; i < 7; ++i)
        {
            const float x = p.x + (i - 3) * 24.f;
            r.Rect(x - 4, p.y - 52, 8, 25, Paper);
            r.Ellipse(x, p.y - 59, 5, 10, Color(1.f, .67f, .2f));
        }
        r.Text(p.x - 70, p.y - 85, "일곱 불 / 여섯 이름", Paper);
    }
    else if (id == Rations)
    {
        for (int i = 0; i < 2 + (std::min)(3, m_State.supplyLoss); ++i)
        {
            m_People.Draw(r,
                          i % 2 ? Chapter3Person::Edric : Chapter3Person::Courier,
                          Chapter3Motion::Idle,
                          time + i * .25f,
                          {p.x + 48 + i * 28.f, p.y + 14});
        }
        r.Text(p.x - 65,
               p.y - 65,
               m_State.supplyLoss >= 2 ? "보급 부족 · 긴 대기" : "제한 배급",
               Paper);
    }
    else if (m_State.hearing && id == RestrictedDoor)
    {
        r.Rect(p.x - 38, p.y - 53, 76, 16, Color(.65f, .17f, .19f));
        r.Text(p.x - 35, p.y - 70, "왕실 봉인", Paper);
    }
    else if (id == ElianDesk)
    {
        if (!m_State.hearing)
        {
            r.Rect(p.x - 20, p.y - 49, 40, 15, Paper);
        }
        r.Text(p.x - 55, p.y - 68, m_State.hearing ? "명패 철거 · 해임" : "학자 엘리안", Paper);
    }
    else if (id == SupplyDesk)
    {
        for (int i = 0; i < (m_State.hearing ? 2 : 5); ++i)
        {
            r.Rect(p.x - 42 + i * 18.f, p.y - 55, 12, 20, Paper);
        }
        r.Text(
            p.x - 65, p.y - 75, m_State.hearing ? "치료품·여비만 지급" : "중앙 지원 대기", Paper);
    }
    if (m_Hold > 0.f && m_HoldAction == id)
    {
        r.Rect(p.x - 30, p.y + 8, 60, 5, Stone);
        r.Rect(p.x - 30, p.y + 8, 60 * std::clamp(m_Hold / 2.f, 0.f, 1.f), 5, Green);
    }
    return true;
}

void Chapter3::AddActors(SceneGraph& scene, WorldPosition) const
{
    const auto add = [&](WorldPosition p, int actor)
    {
        scene.AddDynamicActor({p.x, p.y, 10, 1.f, actor});
    };
    if (m_State.map == Chapter3Maps::Memorial)
    {
        if (!m_State.hearing)
        {
            add({300, -280}, 10);
        }
        add({-100, -500}, 11);
    }
    if (m_State.map == Chapter3Maps::Argen)
    {
        if (m_State.phase == Chapter3Phase::Chase)
        {
            add(m_Courier, 0);
        }
        add({-950, -1650}, 12);
        add({40, -1900}, 11);
        if (!m_State.hearing)
        {
            add({-1100, -1450}, 10);
        }
        const std::array<WorldPosition, 4> starts = {
            {{-600, -2150},
             {600, -2150},
             {0, -2250},
             {m_State.extraCopy.value_or(0) == 1 ? 550.f : -550.f, -2450}}};
        for (int i = 0; i < 4; ++i)
        {
            if (m_Delivery[i] > 0.f && m_Delivery[i] < 4.f)
            {
                auto p = starts[i];
                p.x += (i % 2 == 0 ? -1.f : 1.f) * 80.f * m_Delivery[i];
                p.y -= 25.f * m_Delivery[i];
                add(p, 20 + i);
            }
        }
    }
    if (m_State.map == Chapter3Maps::ReadingRoom)
    {
        if (!m_State.hearing)
        {
            add({150, -1150}, 10);
        }
        add({-120, -1180}, 11);
    }
    if (m_State.map == Chapter3Maps::Archive)
    {
        if (m_State.phase == Chapter3Phase::Infiltration)
        {
            for (int i = 0; i < 2; ++i)
            {
                if (!m_Subdued.test(i))
                {
                    add(Patrol(i, m_PatrolTime), i == 0 ? 1 : 6);
                }
            }
        }
        if (m_State.phase == Chapter3Phase::Interception)
        {
            add(Cart(), 2);
            add({-470, -1300}, 11);
            add({-470, -1400}, 3);
            if (m_AttackTime > 0.f)
            {
                add(m_Ranged ? m_AttackTarget : m_BossPosition, 4);
            }
        }
        if (m_State.seradAlive)
        {
            add(m_BossPosition, 5);
        }
    }
}

void Chapter3::DrawActor(Renderer& r, int actor, Point p, float time) const
{
    if (actor == 11)
    {
        const bool fighting = m_State.map == Chapter3Maps::Archive &&
                              m_State.phase == Chapter3Phase::Interception && !m_State.seradAlive;
        const bool drawn = m_People.Draw(r,
                                         Chapter3Person::Hero,
                                         fighting ? Chapter3Motion::Attack : Chapter3Motion::Idle,
                                         fighting ? std::fmod(time, 1.f) : time,
                                         p);
        r.Text(p.x - 25, p.y - 66, drawn ? "용사" : "용사 이미지 로드 실패", Paper);
    }
    else if (actor == 2)
    {
        Chapter3Props::Draw(r, Chapter3Prop::Cart, p, 62.f);
        const auto motion = m_State.brake ? Chapter3Motion::Idle : Chapter3Motion::Walk;
        m_People.Draw(r, Chapter3Person::Guard, motion, time, {p.x - 52, p.y});
        m_People.Draw(r, Chapter3Person::Guard, motion, time + .3f, {p.x + 52, p.y});
        r.Text(p.x - 90,
               p.y - 72,
               m_State.brake           ? "반출 정지"
               : m_State.wheelsStopped ? "두 번째 운반조 · 감속"
                                       : "첫 운반조 · 원본 이동",
               Paper);
        r.Text(p.x - 70, p.y + 24, "[Space 유지] 바퀴 해제", Green);
        if (m_HoldAction == 500)
        {
            r.Rect(p.x - 30, p.y + 34, 60 * std::clamp(m_Hold / 1.2f, 0.f, 1.f), 5, Green);
        }
    }
    else if (actor == 4)
    {
        CombatCueView::DrawArea(
            r, p, m_Ranged ? 75.f : 105.f, 1.f - m_AttackTime / (m_Ranged ? 1.2f : .9f));
        if (m_AttackNumber % 3 == 0)
        {
            r.Ellipse(p.x, p.y - 15, 75, 45, Color(.42f, .45f, .49f, .35f));
        }
        r.Text(p.x - 70,
               p.y - 105,
               m_Ranged ? "쇠뇌 · 회피/Space 패링" : "검격 · 직전 Space 패링",
               Paper);
    }
    else
    {
        const char* label = actor == 0                 ? "탁본 연락책"
                            : actor == 1 || actor == 6 ? "순찰대 · Space 제압"
                            : actor == 3               ? "호위대 · 용사가 저지"
                            : actor == 5               ? "세라드 · 생존"
                            : actor == 10              ? "엘리안"
                            : actor == 12              ? "에드릭"
                                                       : "공증 사본 운반자";
        const auto person = actor == 10                 ? Chapter3Person::Elian
                            : actor == 12               ? Chapter3Person::Edric
                            : actor == 5                ? Chapter3Person::Serad
                            : actor == 0 || actor >= 20 ? Chapter3Person::Courier
                                                        : Chapter3Person::Guard;
        auto motion = actor == 0 || actor == 1 || actor == 6 || actor >= 20 ? Chapter3Motion::Walk
                                                                            : Chapter3Motion::Idle;
        bool flip = actor == 0 ? m_CourierLeg == 1 : actor >= 20 ? actor % 2 == 0 : false;
        float seconds = time;
        const int guard = actor == 1 ? 0 : actor == 6 ? 1 : -1;
        if (guard >= 0 && guard == m_ThreateningGuard && m_Alarm > 1.f)
        {
            motion = Chapter3Motion::Attack;
            seconds = (m_Alarm - 1.f) * 2.f;
        }
        if (actor == 3)
        {
            motion = Chapter3Motion::Attack;
            seconds = std::fmod(time + .5f, 1.f);
        }
        if (m_People.Draw(r, person, motion, seconds, p, flip))
        {
            r.Text(p.x - 35, p.y - 66, label, Paper);
        }
        else
        {
            Person(r, p, Stone, label);
        }
        if (actor == 1 || actor == 6)
        {
            r.Ellipse(p.x, p.y, 145, 125.5f, Color(.9f, .55f, .16f, .08f));
        }
    }
}

void Chapter3::DrawEnemy(Renderer& r, const FieldEnemy& enemy, Point p) const
{
    const auto motion = enemy.health == 1 ? Chapter3Motion::Idle
                        : m_AttackTime > 0.f
                            ? (m_Ranged ? Chapter3Motion::Ranged : Chapter3Motion::Attack)
                        : m_BossMoving ? Chapter3Motion::Walk
                                       : Chapter3Motion::Idle;
    const float seconds =
        m_AttackTime > 0.f ? 1.f - m_AttackTime / (m_Ranged ? 1.2f : .9f) : m_VisualTime;
    if (m_People.Draw(r, Chapter3Person::Serad, motion, seconds, p, m_BossFaceLeft))
    {
        r.Text(p.x - 35, p.y - 66, "세라드 · 기력", Paper);
    }
    else
    {
        Person(r, p, enemy.hitFlash > 0.f ? Paper : Color(.23f, .3f, .47f), "세라드 · 기력");
    }
    r.Rect(p.x - 38, p.y - 78, 76, 5, Stone);
    r.Rect(p.x - 38,
           p.y - 78,
           76.f * (enemy.health - 1) / (std::max)(1, enemy.maxHealth - 1),
           5,
           Green);
    if (enemy.health == 1)
    {
        r.Text(p.x - 85, p.y + 25, "[Space 유지] 생존 제압", Green);
    }
}

MonitorObservation Chapter3::ObserveMap(const MapDefinition& map, WorldPosition) const
{
    MonitorObservation result;
    result.status = m_State.hearing ? "restricted" : "investigating";
    result.detail = Objective();
    const auto add =
        [&](const char* id, const char* name, const char* role, const char* state, WorldPosition p)
    {
        result.npcs.push_back({id, name, role, state, "챕터 3 서사 액터", true, p});
    };
    if (map.id == Chapter3Maps::Memorial && !m_State.hearing)
    {
        add("elian-hall", "엘리안", "scholar", "investigating", {300, -280});
    }
    if (map.id == Chapter3Maps::Argen)
    {
        add("edric",
            "에드릭",
            "civilian",
            m_State.hearing ? "sanctioned" : "waiting",
            {-950, -1650});
        add("hero", "용사", "companion", "command_retained", {40, -1900});
        if (!m_State.hearing)
        {
            add("elian", "엘리안", "scholar", "investigating", {-1100, -1450});
        }
        if (m_State.phase == Chapter3Phase::Chase)
        {
            add("courier", "탁본 연락책", "courier", "fleeing", m_Courier);
        }
    }
    if (map.id == Chapter3Maps::Archive && m_State.phase == Chapter3Phase::Infiltration)
    {
        result.status = "patrolled";
        for (int i = 0; i < 2; ++i)
        {
            if (!m_Subdued.test(i))
            {
                add(i == 0 ? "patrol-1" : "patrol-2",
                    "기록수호대 순찰",
                    "guard",
                    "patrolling",
                    Patrol(i, m_PatrolTime));
            }
        }
    }
    if (map.id == Chapter3Maps::ReadingRoom)
    {
        add("hero-reading", "용사", "companion", "investigating", {-120, -1180});
        if (!m_State.hearing)
        {
            add("elian-reading", "엘리안", "scholar", "investigating", {150, -1150});
        }
    }
    if (map.id == Chapter3Maps::Archive && m_State.phase == Chapter3Phase::Interception)
    {
        result.status = m_State.brake ? "export_stopped" : "intercepting";
        add("hero-archive", "용사", "companion", "holding_escort", {-470, -1300});
        add("escort", "호위대", "guard", "held_nonlethally", {-470, -1400});
    }
    if (map.id == Chapter3Maps::Archive && m_State.seradAlive)
    {
        add("serad", "세라드", "captain", "subdued_alive", m_BossPosition);
    }
    return result;
}
