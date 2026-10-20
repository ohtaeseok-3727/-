#include "stdafx.h"
#include "Chapter1.h"
#include "SceneGraph.h"
#include "CombatCueView.h"
#include <cmath>

std::string Chapter1::InteractionPrompt(const MapLandmark& site) const
{
    if (site.object == Chapter1Objects::BellA || site.object == Chapter1Objects::BellB ||
        site.object == Chapter1Objects::BellC)
    {
        return "[Ctrl] " + site.name + " 파괴";
    }
    if (site.object == Chapter1Objects::ReinforcementGate)
    {
        return "[Space 유지] " + site.name + " 차단";
    }
    return Chapter::InteractionPrompt(site);
}

void Chapter1::DrawHero(Renderer& r, Point p, float time) const
{
    const int pose =
        m_State.phase == Chapter1Phase::HeroPayoff ? (static_cast<int>(time) % 8 < 4 ? 1 : 2) : 0;
    if (!m_Sprites.Draw(r, ChapterSprite::Hero, pose, p, 50))
    {
        r.Rect(p.x - 12, p.y - 35, 24, 35, Color(.5f, .42f, .35f));
        r.Text(p.x - 25, p.y - 50, "용사(임시)", Color(1, .8f, .4f));
    }
}

void Chapter1::DrawEnemy(Renderer& r, const FieldEnemy& enemy, Point p) const
{
    ChapterSprite sprite =
        enemy.windup > 0 ? ChapterSprite::GoblinAttack : ChapterSprite::GoblinWalk;
    int frame = static_cast<int>(enemy.animationTime * 5) % 4;
    float height = 43;
    if (enemy.role == EnemyRole::Healer)
    {
        sprite = ChapterSprite::Healer;
        frame = enemy.windup > 0 ? 6 : frame;
    }
    else if (enemy.role == EnemyRole::Boss)
    {
        sprite = ChapterSprite::Commander;
        height = 68;
        frame = enemy.health <= 0   ? 15
                : enemy.legInjured  ? 12 + frame % 2
                : enemy.armorBroken ? 8 + frame % 2
                : enemy.windup > 0  ? 4 + frame
                                    : frame;
    }
    CombatCueView::DrawEnemy(r, enemy, p, height);
    r.Ellipse(p.x, p.y, height * .3f, 7, Color(.02f, .03f, .02f, .5f));
    if (!m_Sprites.Draw(r, sprite, frame, p, height, enemy.faceLeft))
    {
        r.Ellipse(p.x, p.y - height * .5f, height * .3f, height * .5f, Color(.47f, .58f, .29f));
        r.Text(p.x - 20, p.y - height - 10, "임시 외형", Color(1, .8f, .4f));
    }
    if (enemy.health <= 0)
    {
        return;
    }
    r.Rect(p.x - 20, p.y - height - 7, 40, 4, Color(.14f, .10f, .10f));
    r.Rect(p.x - 20,
           p.y - height - 7,
           40.f * enemy.health / enemy.maxHealth,
           4,
           Color(.8f, .23f, .14f));
    if (enemy.role == EnemyRole::Boss)
    {
        r.Text(p.x - 45, p.y - height - 16, "그르마 · 고블린 군단장", Color(.95f, .72f, .4f));
    }
    if (enemy.role == EnemyRole::Healer)
    {
        r.Text(p.x - 20, p.y - height - 16, "치료병", Color(.5f, 1, .5f));
        if (enemy.windup > 0)
        {
            r.Ellipse(p.x, p.y, 45, 18, Color(.2f, .8f, .4f, .3f));
        }
    }
    else if (enemy.role == EnemyRole::Ranged)
    {
        r.Text(p.x - 20, p.y - height - 16, "투척병", Color(.8f, .7f, 1));
    }
}

bool Chapter1::DrawLandmark(Renderer& r, const MapLandmark& site, Point p, float time) const
{
    const auto object = site.object;
    const Color wood(.4f, .29f, .17f), gold(.85f, .66f, .25f), dark(.15f, .17f, .18f);
    if (object == Chapter1Objects::Hero)
    {
        if (m_State.grumaDefeated || m_State.chapter1Complete)
        {
            DrawHero(r, p, time);
        }
        return true;
    }
    if (object == Chapter1Objects::Ration || object == Chapter1Objects::Farmer ||
        object == Chapter1Objects::Prisoner)
    {
        if (object == Chapter1Objects::Prisoner && m_State.prisonersReleased)
        {
            r.Text(p.x - 45, p.y - 22, "마을로 귀환한 주민", gold);
            return true;
        }
        if (object == Chapter1Objects::Farmer && m_State.villageAlert)
        {
            p.x += 28;
        }
        if (!m_Sprites.Draw(r, ChapterSprite::Civilian, 0, p, 45))
        {
            r.Rect(p.x - 10, p.y - 35, 20, 35, wood);
        }
        if (object == Chapter1Objects::Ration)
        {
            r.Rect(p.x - 45, p.y + 3, 90, 14, wood);
            r.Text(p.x - 45, p.y - 55, "빈 주민 배급소", gold);
        }
        if (object == Chapter1Objects::Prisoner)
        {
            r.Rect(p.x - 10, p.y - 17, 20, 3, gold);
        }
        return true;
    }
    if (object == Chapter1Objects::BellA || object == Chapter1Objects::BellB ||
        object == Chapter1Objects::BellC)
    {
        const int index = object == Chapter1Objects::BellA   ? 0
                          : object == Chapter1Objects::BellB ? 1
                                                             : 2;
        const bool destroyed = m_State.bellHealth[index] <= 0;
        r.Rect(p.x - 30, p.y - 60, 5, 60, wood);
        r.Rect(p.x + 25, p.y - 60, 5, 60, wood);
        r.Rect(p.x - 30, p.y - 60, 60, 6, wood);
        if (destroyed)
        {
            r.Triangle({p.x - 18, p.y}, {p.x, p.y - 12}, {p.x + 7, p.y + 3}, gold);
            r.Rect(p.x + 10, p.y - 4, 14, 5, gold);
        }
        else
        {
            r.Quad({p.x - 10, p.y - 50},
                   {p.x + 10, p.y - 50},
                   {p.x + 20, p.y - 23},
                   {p.x - 20, p.y - 23},
                   gold);
            r.Rect(p.x - 25,
                   p.y - 20,
                   50.f * m_State.bellHealth[index] / 60,
                   3,
                   Color(.8f, .3f, .15f));
        }
        r.Text(p.x - 25, p.y - 70, destroyed ? "파괴된 종" : "지휘 종 [Ctrl]", gold);
        return true;
    }
    if (object == Chapter1Objects::ReinforcementGate)
    {
        r.Rect(p.x - 25, p.y - 25, 50, 25, wood);
        r.Ellipse(p.x, p.y - 32, 16, 16, gold);
        r.Rect(p.x - 90, p.y - 140, 180, 12, wood);
        for (int i = -3; i <= 3; ++i)
        {
            r.Rect(p.x + i * 22, p.y - 135, 7, m_State.reinforcementGateClosed ? 65 : 15, dark);
        }
        r.Text(p.x - 65,
               p.y - 50,
               m_State.reinforcementGateClosed ? "증원문 닫힘 · 군단 고립"
                                               : "낙하 장치 [Space 유지]",
               gold);
        r.Rect(p.x - 40, p.y + 10, 80 * m_State.gateProgress / 4, 5, Color(.4f, .8f, .5f));
        return true;
    }
    if (object == Chapter1Objects::Wagon)
    {
        r.Rect(p.x - 45, p.y - 32, 90, 26, wood);
        for (int side : {-1, 1})
        {
            r.Ellipse(p.x + side * 34, p.y, 12, 12, dark);
        }
        for (int i = 0; i < 3; ++i)
        {
            r.Rect(p.x - 35 + i * 23, p.y - 52, 21, 22, gold);
        }
        r.Rect(p.x - 4,
               p.y - 15,
               m_State.convoyDisabled ? 16 : 5,
               5,
               m_State.convoyDisabled ? Color(.9f, .2f, .1f) : dark);
        r.Text(p.x - 55,
               p.y - 65,
               m_State.convoyDisabled ? "고정핀 제거 · 수레 정지" : "군수품 / 포로 수송 수레",
               gold);
        return true;
    }
    if (object == Chapter1Objects::LightTracks || object == Chapter1Objects::HeavyTracks ||
        object == Chapter1Objects::Grain || object == Chapter1Objects::Bark ||
        object == Chapter1Objects::Rope)
    {
        for (int i = 0; i < 6; ++i)
        {
            const float thickness = object == Chapter1Objects::HeavyTracks ? 5.f : 2.f;
            const Color clue = object == Chapter1Objects::Grain  ? gold
                               : object == Chapter1Objects::Rope ? Color(.7f, .65f, .45f)
                                                                 : wood;
            r.Rect(p.x - 22 + i * 9, p.y - i * 3, 7, thickness, clue);
            if (object == Chapter1Objects::LightTracks || object == Chapter1Objects::HeavyTracks)
            {
                r.Rect(p.x - 22 + i * 9, p.y + 14 - i * 3, 7, thickness, clue);
            }
        }
        r.Text(p.x - 45, p.y - 20, site.name, Color(.7f, .8f, .7f));
        return true;
    }
    if (object == Chapter1Objects::FoodEvidence || object == Chapter1Objects::RepairEvidence ||
        object == Chapter1Objects::TransportEvidence || object == Chapter1Objects::MilitarySupply ||
        object == Chapter1Objects::Storage || object == Chapter1Objects::Donation)
    {
        r.Rect(p.x - 24, p.y - 28, 48, 28, wood);
        r.Rect(p.x - 19,
               p.y - 25,
               38,
               18,
               object == Chapter1Objects::MilitarySupply ? Color(.32f, .4f, .25f) : dark);
        r.Text(p.x - 55, p.y - 40, site.name, gold);
        if (m_State.villageAlert && object == Chapter1Objects::FoodEvidence)
        {
            r.Rect(p.x + 26, p.y, 22, 5, wood);
        }
        return true;
    }
    if (object == Chapter1Objects::Ledger || object == Chapter1Objects::CommandSeal ||
        object == Chapter1Objects::Invoice)
    {
        r.Rect(p.x - 18, p.y - 30, 36, 30, wood);
        r.Rect(p.x - 14, p.y - 32, 28, 22, Color(.75f, .7f, .5f));
        r.Ellipse(p.x + 5, p.y - 20, 5, 5, Color(.7f, .2f, .1f));
        r.Text(p.x - 35, p.y - 44, site.name, gold);
        return true;
    }
    return false;
}

void Chapter1::DrawWeather(Renderer& r, float time, int width, int height) const
{
    if (m_State.map != Chapter1Maps::Forest)
    {
        return;
    }
    for (int i = 0; i < 65; ++i)
    {
        const float x = std::fmod(i * 127.f + time * 35, float(width));
        const float y = std::fmod(i * 83.f + time * 280, float(height));
        r.Rect(x, y, 1, 9, Color(.55f, .7f, .75f, .28f));
    }
}

bool Chapter1::EnemyVisible(const FieldEnemy& enemy) const
{
    return enemy.health > 0 || (enemy.role == EnemyRole::Boss && m_State.grumaDefeated);
}

void Chapter1::AddActors(SceneGraph& scene, WorldPosition player) const
{
    if (m_State.heroArrived && ControlLocked())
    {
        // Kind 10 is reserved for chapter actors; entity identifies the local actor.
        scene.AddDynamicActor({player.x + 42.f, player.y + 8.f, 10, 1.f, 0});
    }
}

void Chapter1::DrawActor(Renderer& renderer, int actor, Point p, float time) const
{
    if (actor == 0)
    {
        DrawHero(renderer, p, time);
    }
}
