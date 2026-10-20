#include "stdafx.h"
#include "Chapter2.h"
#include "Renderer.h"
#include "SceneGraph.h"
#include "CombatCueView.h"
#include <algorithm>
#include <cmath>

namespace
{
    Color ConditionColor(Chapter2Condition condition)
    {
        switch (condition)
        {
        case Chapter2Condition::Intact:
            return Color(.32f, .68f, .48f);
        case Chapter2Condition::Damaged:
            return Color(.88f, .61f, .24f);
        case Chapter2Condition::Critical:
            return Color(.83f, .29f, .25f);
        }
        return Color(.7f, .7f, .7f);
    }

    const char* ConditionName(Chapter2Condition condition)
    {
        switch (condition)
        {
        case Chapter2Condition::Intact:
            return "온전함";
        case Chapter2Condition::Damaged:
            return "일부 피해";
        case Chapter2Condition::Critical:
            return "피해 억제만 가능";
        }
        return "";
    }
}

std::string Chapter2::InteractionPrompt(const MapLandmark& site) const
{
    switch (site.object)
    {
    case Chapter2Objects::CommandTarget:
        return Has(Chapter2Event::ArmorBroken) ? "[Ctrl] 지휘 인장 파괴"
                                               : "지휘 인장 · 바르칸 약화 필요";
    case Chapter2Objects::Mural:
        return "[Space 유지] 벽화 탁본 · 10초";
    case Chapter2Objects::DemolitionLine:
    case Chapter2Objects::Bypass:
    case Chapter2Objects::RetreatChain:
    case Chapter2Objects::BossDemolition:
    case Chapter2Objects::BeaconControl:
    case Chapter2Objects::WorkshopControl:
    case Chapter2Objects::CampControl:
        return "[Space 유지] " + site.name + " · 경계 처치 후";
    default:
        return Chapter::InteractionPrompt(site);
    }
}

bool Chapter2::DrawLandmark(Renderer& r, const MapLandmark& site, Point p, float time) const
{
    const Color stone(.35f, .39f, .42f), gold(.9f, .77f, .48f);
    r.Rect(p.x - 22.f, p.y - 28.f, 44.f, 28.f, stone);
    r.Rect(p.x - 18.f, p.y - 34.f, 36.f, 6.f, gold);
    if (site.object == Chapter2Objects::BeaconControl ||
        site.object == Chapter2Objects::WorkshopControl ||
        site.object == Chapter2Objects::CampControl)
    {
        const auto target = site.object == Chapter2Objects::BeaconControl ? Chapter2Target::Beacon
                            : site.object == Chapter2Objects::WorkshopControl
                                ? Chapter2Target::Workshop
                                : Chapter2Target::Camp;
        const auto index = static_cast<std::size_t>(target);
        const auto color = ConditionColor(m_State.conditions[index]);
        const bool resolved = m_State.resolved.test(index);
        if (target == Chapter2Target::Beacon)
        {
            r.Rect(p.x - 18.f, p.y - 125.f, 36.f, 95.f, stone);
            r.Ellipse(p.x, p.y - 132.f, 27.f, 13.f, resolved ? stone : color);
        }
        else if (target == Chapter2Target::Workshop)
        {
            r.Rect(p.x - 65.f, p.y - 48.f, 130.f, 14.f, stone);
            for (int i = -1; i <= 1; ++i)
            {
                r.Rect(p.x + i * 38.f - 12.f, p.y - 76.f, 24.f, 28.f, color);
            }
        }
        else
        {
            for (int i = -3; i <= 3; ++i)
            {
                r.Rect(p.x + i * 16.f, p.y - 78.f, 6.f, 55.f, color);
            }
            r.Rect(p.x - 48.f, p.y - 72.f, 102.f, 6.f, stone);
        }
        r.Text(p.x - 55.f,
               p.y - 82.f,
               resolved ? "대응 완료" : ConditionName(m_State.conditions[index]),
               color);
        if (!resolved && m_State.conditions[index] != Chapter2Condition::Intact)
        {
            const float rise = std::fmod(time * 22.f, 30.f);
            r.Ellipse(p.x, p.y - 70.f - rise, 16.f, 10.f, Color(.35f, .29f, .25f, .6f));
        }
    }
    else if (site.object == Chapter2Objects::Aqueduct || site.object == Chapter2Objects::Chapel ||
             site.object == Chapter2Objects::Breach)
    {
        const auto route = static_cast<Chapter2Route>(site.object - Chapter2Objects::Aqueduct);
        const bool available = RouteAvailable(route);
        r.Rect(p.x - 55.f,
               p.y - 65.f,
               110.f,
               65.f,
               available ? Color(.24f, .53f, .4f) : Color(.44f, .24f, .22f));
        r.Text(p.x - 50.f, p.y - 78.f, available ? "선택 가능" : "조건 미충족", gold);
    }
    else if (site.object == Chapter2Objects::Mural)
    {
        r.Rect(p.x - 135.f, p.y - 135.f, 270.f, 105.f, Color(.42f, .4f, .35f));
        for (int i = 0; i < 7; ++i)
        {
            const float x = p.x - 112.f + i * 35.f;
            r.Rect(x, p.y - 93.f, 17.f, 37.f, Color(.62f, .58f, .44f));
            r.Ellipse(x + 8.f, p.y - 107.f, 8.f, 9.f, Color(.7f, .63f, .49f));
            if (i == 6)
            {
                r.Rect(x - 2.f, p.y - 116.f, 22.f, 20.f, stone);
                r.Rect(x - 2.f, p.y - 48.f, 22.f, 4.f, stone);
                r.Rect(x + 15.f, p.y - 80.f, 18.f, 4.f, gold);
                r.Ellipse(x + 26.f, p.y - 57.f, 12.f, 5.f, Color(.36f, .48f, .34f));
            }
        }
        r.Text(p.x - 125.f, p.y - 148.f, "일곱 귀환자 · 지워진 얼굴과 이름", gold);
    }
    else if (site.object == Chapter2Objects::ReliefReport && m_State.outcome.barkanDefeated)
    {
        // Symbols communicate relative outcomes; these are not literal casualty counts.
        for (int i = 0; i < 6; ++i)
        {
            const bool occupied = i < 6 - m_State.outcome.refugeeLoss;
            r.Rect(p.x - 85.f + i * 28.f,
                   p.y - 75.f,
                   19.f,
                   occupied ? 30.f : 4.f,
                   occupied ? Color(.5f, .66f, .5f) : Color(.4f, .34f, .3f));
        }
        for (int i = 0; i < 4; ++i)
        {
            r.Rect(p.x - 85.f + i * 38.f,
                   p.y + 20.f,
                   29.f,
                   18.f,
                   i < 4 - m_State.outcome.supplyLoss ? gold : stone);
        }
        r.Text(p.x - 100.f, p.y - 105.f, "남겨진 자리와 살아 돌아온 사람들", gold);
        r.Text(p.x - 100.f,
               p.y + 65.f,
               "교량 손실 " + std::to_string(m_State.outcome.bridgeDamage) + " / 증원 압박 " +
                   std::to_string(m_State.outcome.eliteReinforcement),
               gold);
    }
    r.Text(p.x - 75.f, p.y + 14.f, site.name, gold);
    return true;
}

void Chapter2::DrawEnemy(Renderer& r, const FieldEnemy& enemy, Point p) const
{
    m_Sprites.DrawEnemy(r, enemy, p);
}

void Chapter2::DrawActor(Renderer& r, int actor, Point p, float time) const
{
    if (actor == 1 || actor == 2)
    {
        const float radius = actor == 2                                      ? 95.f
                             : m_Combat.bossAction == EnemyAction::JumpSlam  ? 110.f
                             : m_Combat.bossAction == EnemyAction::BattleCry ? 185.f
                                                                             : 85.f;
        const float progress =
            actor == 2 ? 1.f - m_Combat.explosionWarning / 1.5f : m_Combat.bossProgress;
        CombatCueView::DrawArea(r, p, radius, progress);
        r.Rect(p.x - 35.f, p.y + radius * .8660254f + 5.f, 70.f, 4.f, Color(.14f, .09f, .06f));
        r.Rect(p.x - 35.f,
               p.y + radius * .8660254f + 5.f,
               70.f * std::clamp(progress, 0.f, 1.f),
               4.f,
               Color(1.f, .55f, .12f));
        r.Text(p.x - 42.f,
               p.y - 18.f,
               actor == 2                                      ? "폭발 임박"
               : m_Combat.bossAction == EnemyAction::JumpSlam  ? "내려찍기"
               : m_Combat.bossAction == EnemyAction::BattleCry ? "전투의 함성"
                                                               : "휘두르기",
               Color(1.f, .8f, .4f));
    }
    else if (actor == 3)
    {
        r.Rect(p.x - 120.f, p.y - 20.f, 240.f, 36.f, Color(.3f, .26f, .21f));
        for (int i = -3; i <= 3; ++i)
        {
            if (i == m_State.outcome.bridgeDamage)
            {
                continue;
            }
            r.Rect(p.x + i * 30.f, p.y - 24.f, 24.f, 44.f, Color(.55f, .43f, .3f));
        }
        r.Text(p.x - 85.f, p.y - 42.f, "부서진 교량 · 남은 통로", Color(.9f, .75f, .5f));
    }
    else if (actor == 4)
    {
        r.Ellipse(p.x, p.y, 35.f, 17.f, Color(.25f, .85f, .65f, .65f));
        r.Rect(p.x - 12.f, p.y - 35.f, 24.f, 30.f, Color(.6f, .65f, .52f));
        r.Text(p.x - 40.f, p.y - 48.f, "현재 대응 위치", Color(.8f, 1.f, .8f));
    }
    else if (actor == 5)
    {
        r.Rect(p.x - 100.f, p.y - 65.f, 200.f, 65.f, Color(.36f, .29f, .22f));
        for (int i = -4; i <= 4; ++i)
        {
            r.Rect(p.x + i * 22.f, p.y - 75.f, 7.f, 80.f, Color(.6f, .54f, .42f));
        }
    }
    else if (actor == 0)
    {
        m_Sprites.DrawHero(r, p, time, m_Combat.heroAttack > 0.f, m_Combat.heroFaceLeft);
    }
}

bool Chapter2::EnemyVisible(const FieldEnemy& enemy) const
{
    return enemy.health > 0 || enemy.stats.preserveCorpse || enemy.DeathAnimationPlaying();
}

void Chapter2::AddActors(SceneGraph& scene, WorldPosition) const
{
    if (m_Combat.bossAction != EnemyAction::None && m_State.phase == Chapter2Phase::Boss)
    {
        const auto p = m_Combat.bossAction == EnemyAction::JumpSlam ? m_Combat.bossTarget
                                                                    : m_Combat.bossPosition;
        scene.AddDynamicActor({p.x, p.y, 10, 1.f, 1});
    }
    if (m_Combat.explosionWarning > 0.f)
    {
        const auto p = m_Combat.explosionPosition;
        scene.AddDynamicActor({p.x, p.y, 10, 1.f, 2});
    }
    if (m_State.map == Chapter2Maps::Canyon && m_State.outcome.barkanDefeated)
    {
        scene.AddDynamicActor({650, 450, 10, 1.f, 3});
    }
    if (m_State.map == Chapter2Maps::Canyon && m_State.activeTarget)
    {
        const auto p = DevicePosition(*m_State.activeTarget, m_Combat.objectiveStep);
        scene.AddDynamicActor({p.x, p.y, 10, 1.f, 4});
    }
    if (m_State.map == Chapter2Maps::OldFortress)
    {
        for (const auto p : {WorldPosition{-360, -945},
                             WorldPosition{0, -945},
                             WorldPosition{360, -945},
                             WorldPosition{0, -1480},
                             WorldPosition{-225, -2540},
                             WorldPosition{225, -2540}})
        {
            if (BlocksMovement(p))
            {
                scene.AddDynamicActor({p.x, p.y, 10, 1.f, 5});
            }
        }
    }
    if (m_Combat.heroPresent && m_State.phase != Chapter2Phase::Complete)
    {
        scene.AddDynamicActor({m_Combat.hero.x, m_Combat.hero.y, 10, 1.f, 0});
    }
}

void Chapter2::DrawWeather(Renderer& r, float, int width, int) const
{
    if (ContinuousWorld())
    {
        return;
    }
    if (width < 760)
    {
        r.Text(16.f,
               142.f,
               "챕터 2 · 전투 위험 안내를 보려면 창을 넓혀 주세요.",
               Color(1.f, .8f, .5f));
        return;
    }
    const float panel = (std::min)(float(width - 32), 1180.f);
    const bool choosing = m_State.map == Chapter2Maps::Canyon && !m_State.outcome.barkanDefeated;
    r.Rect(16.f, 120.f, panel, choosing ? 188.f : 35.f, Color(.03f, .045f, .05f, .93f));
    r.Text(25.f, 142.f, "챕터 2 · 오크 전투와 목표 대응 · Space 상호작용", Color(.9f, .8f, .53f));
    if (choosing)
    {
        for (int i = 0; i < 3; ++i)
        {
            const auto target = static_cast<Chapter2Target>(i);
            const auto condition = m_State.conditions[static_cast<std::size_t>(i)];
            std::string text = std::string(TargetName(target)) + " / " + ConditionName(condition);
            if (m_State.resolved.test(static_cast<std::size_t>(i)))
            {
                text = "[대응 완료] " + text;
            }
            else if (m_State.activeTarget == target)
            {
                text = "[대응 중] " + text;
            }
            r.Text(25.f, 167.f + i * 48.f, text, ConditionColor(condition));
            r.Text(25.f, 187.f + i * 48.f, TargetRisk(target), Color(.84f, .84f, .8f));
        }
    }
}
