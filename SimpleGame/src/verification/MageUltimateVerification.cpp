#include "stdafx.h"
#include "MageUltimateVerification.h"
#include "MageUltimate.h"
#include "SkillSystem.h"
#include "GameSession.h"
#include <cmath>
#include <iostream>
#include <limits>
#include <stdexcept>

namespace
{
    void Require(bool condition, const char* message)
    {
        if (!condition)
            throw std::runtime_error(message);
    }

    void BindUltimate(SkillSystem& skills, int level, int slot)
    {
        const auto& list = skills.PlayerSkillsState().Available();
        const auto layout = SkillLayout::Create(1280, 720, static_cast<int>(list.size()));
        int card = 0;
        while (card < static_cast<int>(list.size()) && list[card].id != "mage_sword_song")
            ++card;
        Require(card < static_cast<int>(list.size()), "Missing ultimate card");
        Require(layout.CardY(card) + 94 < layout.panelY + 330, "Ultimate card outside panel");
        skills.ToggleWindow();
        skills.MouseButton(true,
                           static_cast<int>(layout.CardX(card) + 22),
                           static_cast<int>(layout.CardY(card) + 28),
                           1280,
                           720,
                           level);
        skills.MouseButton(false,
                           static_cast<int>(layout.SlotX(slot) + 10),
                           static_cast<int>(layout.slotsY + 10),
                           1280,
                           720,
                           level);
        skills.CloseWindow();
    }
}

int RunMageUltimateVerification()
{
    try
    {
        PlayerProgression progress;
        progress.Reset(0);
        progress.DebugSetLevel(20);
        SkillSystem skills;
        skills.Reset(0);
        const auto* data = skills.PlayerSkillsState().Find("mage_sword_song");
        Require(data && !skills.PlayerSkillsState().CanUse(*data, 19) &&
                    skills.PlayerSkillsState().CanUse(*data, 20),
                "Ultimate level gate");
        const auto request = skills.BuildRequest(*data, progress, 7);
        const float attack = progress.EffectiveAttack(7),
                    intelligence = static_cast<float>(progress.Intelligence());
        Require(std::abs(request.calculatedDamage - (attack * .1f + intelligence * .2f)) < .001f,
                "Rain damage formula");
        Require(std::abs(request.calculatedBurstDamage - (attack * 4.f + intelligence * 6.f)) <
                    .001f,
                "Explosion damage formula");
        BindUltimate(skills, 19, 3);
        Require(!skills.IsSlotBound('f'), "Locked ultimate was bound");
        BindUltimate(skills, 20, 3);
        BindUltimate(skills, 20, 4);
        Require(skills.IsSlotBound('f') && skills.IsSlotBound('q'), "Ultimate drag registration");
        std::string notice;
        progress.SpendMana(progress.Mana() - 49);
        skills.TryUse('f', progress, notice);
        Require(skills.LastRequest().skillId.empty() && progress.Mana() == 49,
                "MP49 must fail without spending");
        progress.Restore();
        const int mana = progress.Mana();
        skills.TryUse('f', progress, notice);
        Require(progress.Mana() == mana - 50 && skills.Cooldowns().at(data->id) == 60.f,
                "MP50/cooldown60");
        skills.TryUse('q', progress, notice);
        Require(skills.LastRequest().skillId.empty(), "Cooldown must be shared by slots");
        skills.Update(60.f);
        progress.Restore();
        skills.TryUse('q', progress, notice);
        Require(skills.LastRequest().skillId == data->id, "Cooldown did not finish");

        EnemySystem enemies;
        EnemyStats stats;
        stats.health = 10000;
        for (const auto position : {WorldPosition{0, 0},
                                    WorldPosition{300, 0},
                                    WorldPosition{301, 0},
                                    WorldPosition{210, 210},
                                    WorldPosition{225, 225},
                                    WorldPosition{-300, 0}})
            enemies.Add(position, 0, EnemyRole::Melee, stats);
        for (int pulse = 0; pulse < 40; ++pulse)
            enemies.StrikeArea({0, 0}, 300, 3);
        enemies.StrikeArea({0, 0}, 300, 100);
        const auto& targets = enemies.Enemies();
        for (int i = 0; i < 6; ++i)
            Require(targets[i].health == ((i == 2 || i == 4) ? 10000 : 9780),
                    "Circular membership or repeated area damage");
        enemies.StrikeArea({400, 0}, 300, 7);
        Require(enemies.Enemies()[0].health == 9780, "Enemy outside circle still damaged");
        enemies.StrikeArea({0, 0}, 300, 7);
        Require(enemies.Enemies()[0].health == 9773, "Re-entering circle must receive later ticks");
        enemies.Clear();
        stats.health = 5;
        enemies.Add({0, 0}, 0, EnemyRole::Melee, stats);
        Require(enemies.StrikeArea({0, 0}, 300, 8).size() == 1 &&
                    enemies.StrikeArea({0, 0}, 300, 8).empty(),
                "Repeated kill reward");

        LevelMap map;
        GameSession session;
        session.Reset(0, 743, map);
        MageUltimate skill;
        skill.Cast(request, {100, 200}, true);
        Require(skill.Casting() && skill.Center().x == 100 && skill.FaceLeft(), "Cast state");
        skill.Update(.799f, session);
        Require(skill.PulsesApplied() == 0, "Rain damaged before first tick");
        skill.Update(.002f, session);
        Require(skill.PulsesApplied() == 1, "First rain tick missing");
        skill.Update(std::numeric_limits<float>::quiet_NaN(), session);
        Require(skill.PulsesApplied() == 1, "NaN time changed events");
        skill.Update(3.9f, session);
        Require(skill.PulsesApplied() == 40 && !skill.Exploded(),
                "Exactly 40 hits must end before merge explosion");
        skill.Update(.55f, session);
        Require(skill.Exploded() && !skill.Casting(), "Final explosion missing");
        skill.Update(10.f, session);
        Require(!skill.Active() && skill.PulsesApplied() == 40, "Expired skill repeated damage");
        skill.Cast(request, {0, 0}, false);
        for (int i = 0; i < 581; ++i)
            skill.Update(.01f, session);
        Require(skill.PulsesApplied() == 40 && skill.Exploded() && !skill.Active(),
                "Small frame timing disagrees");
        skill.Cast(request, {0, 0}, false);
        skill.Update(.9f, session);
        skill.Reset();
        skill.Update(10.f, session);
        Require(!skill.Active() && !skill.Exploded() && skill.PulsesApplied() == 0,
                "Cancelled channel fired later");
        skills.Reset(1);
        Require(!skills.PlayerSkillsState().CanUse(*data, 20), "Knight can use mage ultimate");
        std::cout << "마법과 검의 노래 검증 통과: 수치·슬롯·40회·원형 범위·합체 폭발·취소\n";
        return 0;
    }
    catch (const std::exception& error)
    {
        std::cerr << "마법사 궁극기 검증 실패: " << error.what() << '\n';
        return 1;
    }
}
