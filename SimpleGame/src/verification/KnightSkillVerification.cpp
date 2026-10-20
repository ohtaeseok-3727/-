#include "stdafx.h"
#include "KnightSkillVerification.h"
#include "KnightSkills.h"
#include "KnightSkillData.h"
#include "SkillSystem.h"
#include "GameSession.h"
#include <cmath>
#include <iostream>
#include <stdexcept>

namespace
{
    void Require(bool condition, const char* message)
    {
        if (!condition)
        {
            throw std::runtime_error(message);
        }
    }

    void Bind(SkillSystem& skills, int card, char slot, int level)
    {
        const auto layout = SkillLayout::Create(1280, 720);
        const std::string keys = "asdfqwer";
        const int index = static_cast<int>(keys.find(slot));
        skills.ToggleWindow();
        skills.MouseButton(true,
                           static_cast<int>(layout.CardX(card) + 22),
                           static_cast<int>(layout.CardY(card) + 28),
                           1280,
                           720,
                           level);
        skills.MouseButton(false,
                           static_cast<int>(layout.SlotX(index) + 10),
                           static_cast<int>(layout.slotsY + 10),
                           1280,
                           720,
                           level);
        skills.CloseWindow();
    }
}

int RunKnightSkillVerification()
{
    try
    {
        PlayerProgression progress;
        progress.Reset(1);
        SkillSystem skills;
        skills.Reset(1);
        const auto* thrust = skills.PlayerSkillsState().Find("knight_thrust");
        const auto* wave = skills.PlayerSkillsState().Find("knight_wave");
        const auto* aura = skills.PlayerSkillsState().Find("knight_aura");
        Require(thrust && wave && aura, "Missing knight catalog");
        Require(!skills.PlayerSkillsState().CanUse(*thrust, 2) &&
                    skills.PlayerSkillsState().CanUse(*thrust, 3),
                "Thrust level gate");
        Require(!skills.PlayerSkillsState().CanUse(*aura, 20), "Passive must not be castable");
        Require(!skills.PlayerSkillsState().CanUse(*wave, 4) &&
                    skills.PlayerSkillsState().CanUse(*wave, 5),
                "Wave requires aura level");
        progress.DebugSetLevel(3);
        const auto base = skills.BuildRequest(*thrust, progress, 2);
        Require(std::abs(base.calculatedDamage -
                         ((progress.Attack() + 2) * .70f + progress.Strength() * .5f)) < .001f,
                "Thrust damage");
        progress.DebugSetLevel(5);
        Require(progress.KnightAuraActive() && std::abs(progress.AttackRangeScale() - 1.3f) < .001f,
                "Aura activation");
        const auto request = skills.BuildRequest(*wave, progress, 2);
        Require(std::abs(request.calculatedDamage -
                         ((progress.Attack() + 2) * 1.2f * 1.5f + progress.Strength())) < .001f,
                "Wave passive counted incorrectly");
        Bind(skills, 0, 'a', 5);
        Bind(skills, 0, 's', 5);
        Bind(skills, 1, 'd', 5);
        Require(skills.IsSlotBound('a') && skills.IsSlotBound('s') && !skills.IsSlotBound('d'),
                "Slot/passive binding");
        std::string notice;
        const int mana = progress.Mana();
        skills.TryUse('a', progress, notice);
        Require(progress.Mana() == mana - 4 && skills.LastRequest().skillId == "knight_thrust",
                "Thrust MP/use");
        skills.TryUse('s', progress, notice);
        Require(skills.LastRequest().skillId.empty() && progress.Mana() == mana - 4,
                "Shared cooldown failed");
        skills.Update(5.f);
        progress.SpendMana(progress.Mana());
        skills.TryUse('a', progress, notice);
        Require(skills.LastRequest().skillId.empty() && skills.Cooldowns().empty(),
                "MP failure started cooldown");
        progress.Reset(0);
        progress.DebugSetLevel(20);
        Require(!progress.KnightAuraActive() && progress.AttackRangeScale() == 1.f,
                "Aura leaked into mage");

        LevelMap map;
        map.Generate(743);
        GameSession session;
        session.Reset(1, 743, map);
        MapDefinition clear;
        clear.id = 0;
        clear.title = "기사 스킬 검증 전용";
        clear.areas = {{MapRegion::Village, "검증", {0, 0}, 1000, 1000}};
        clear.roads = {{{-2000, 0}, {2000, 0}, 200}};
        map.Load("verification", clear);
        KnightSkills runtime;
        runtime.Cast(base, {0, 0}, false, 1.f);
        Require(runtime.Shots().size() == 6 && runtime.Casting(),
                "Single cast must create six events");
        runtime.Update(.179f, session, map);
        Require(!runtime.Shots()[0].Visible(), "Thrust fired before 180ms");
        runtime.Update(.002f, session, map);
        Require(runtime.Shots()[0].Visible() && !runtime.Shots()[1].Visible(),
                "Staggered emission");
        runtime.Update(.300f, session, map);
        Require(runtime.Shots().empty() && runtime.Casting(), "Effects and recovery lifetime");
        runtime.Update(.130f, session, map);
        Require(!runtime.Casting(), "One thrust motion must end at 610ms");
        runtime.Cast(request, {0, 0}, true, 1.3f);
        runtime.Update(.700f, session, map);
        Require(!runtime.Casting() && runtime.Shots().size() == 1 &&
                    runtime.Shots()[0].position.x < runtime.Shots()[0].origin.x,
                "Wave must outlive motion and move left");
        runtime.Reset();
        Require(!runtime.Casting() && runtime.Shots().empty(), "Reset retained an attack");

        EnemySystem enemies;
        enemies.Add({110, 0}, 1);
        enemies.Strike({0, 0}, false, 1, map);
        Require(enemies.Enemies()[0].health == LevelTuning::EnemyHealth, "Unbuffed range changed");
        enemies.Strike({0, 0}, false, 1, map, 1.3f);
        Require(enemies.Enemies()[0].health == LevelTuning::EnemyHealth - 1, "Buffed basic range");
        enemies.Clear();
        enemies.Add({60, 0}, 1);
        enemies.Add({100, 0}, 2);
        std::unordered_set<std::uint64_t> hits;
        enemies.StrikeProjectile({0, 0}, {130, 0}, 8, 8, 5, hits, map, 35.f, WorldPosition{0, 0});
        enemies.StrikeProjectile({0, 0}, {130, 0}, 8, 8, 5, hits, map, 35.f, WorldPosition{0, 0});
        Require(hits.size() == 2 && enemies.Enemies()[0].health == LevelTuning::EnemyHealth - 5 &&
                    enemies.Enemies()[1].health == LevelTuning::EnemyHealth - 5,
                "Piercing or repeat-hit guard");
        // Ultimate regression: prepare once, hold a harmless afterimage, then release.
        progress.Reset(1);
        progress.DebugSetLevel(20);
        skills.Reset(1);
        const auto* ultimate = skills.PlayerSkillsState().Find("knight_cheondan");
        Require(ultimate && !skills.PlayerSkillsState().CanUse(*ultimate, 19) &&
                    skills.PlayerSkillsState().CanUse(*ultimate, 20),
                "Ultimate level gate");
        const auto ultimateRequest = skills.BuildRequest(*ultimate, progress, 2);
        Require(std::abs(ultimateRequest.calculatedDamage -
                         (progress.EffectiveAttack(2) * 5.f + progress.Strength() * 5.f)) < .001f,
                "Ultimate damage");
        Bind(skills, 3, 'f', 20);
        Bind(skills, 3, 'q', 20);
        progress.SpendMana(progress.Mana() - 29);
        skills.TryUse('f', progress, notice);
        Require(skills.LastRequest().skillId.empty() && progress.Mana() == 29,
                "Ultimate requires MP30");
        progress.Restore();
        const int beforeUltimate = progress.Mana();
        skills.TryUse('f', progress, notice);
        Require(progress.Mana() == beforeUltimate - 30 &&
                    skills.Cooldowns().at("knight_cheondan") == 60.f,
                "Ultimate resource costs");
        skills.Update(59.9f);
        skills.TryUse('q', progress, notice);
        Require(skills.LastRequest().skillId.empty(), "Ultimate shared cooldown ended early");
        skills.Update(.11f);
        skills.TryUse('q', progress, notice);
        Require(skills.LastRequest().skillId == "knight_cheondan", "Ultimate did not recover");
        runtime.Reset();
        runtime.Cast(ultimateRequest, {0, 0}, false, 1.3f);
        runtime.Update(KnightSkillData::UltimateRelease - .01f, session, map);
        Require(runtime.Shots().size() == 1 && !runtime.Shots()[0].Visible() &&
                    runtime.Shots()[0].hitTargets.empty(),
                "Charge/afterimage dealt early damage");
        runtime.Update(.02f, session, map);
        Require(runtime.Shots().size() == 1 && runtime.Shots()[0].Visible(),
                "Ultimate burst missing");
        const auto ultimateBounds = runtime.Shots()[0].Bounds();
        auto reference = runtime.Shots()[0];
        reference.kind = KnightSkillKind::Wave;
        reference.age = reference.delay + .2f;
        const auto waveBounds = reference.Bounds();
        Require(std::abs(ultimateBounds.bottom - waveBounds.bottom) < .001f &&
                    std::abs((ultimateBounds.bottom - ultimateBounds.top) -
                             5.f * (waveBounds.bottom - waveBounds.top)) < .001f &&
                    std::abs((ultimateBounds.right - ultimateBounds.left) -
                             (waveBounds.right - waveBounds.left)) < .001f,
                "Ultimate must grow upwards 5x without widening or lowering the bottom");
        runtime.Update(.40f, session, map);
        Require(!runtime.Casting() && runtime.Shots().size() == 1,
                "Ultimate wave must outlive player motion");
        Require(std::abs(runtime.Shots()[0].distance - KnightSkillData::WaveDistance * 1.3f * 2.f *
                                                           .41f / KnightSkillData::UltimateFlight) <
                    .1f,
                "Ultimate travel does not follow doubled range");
        const float fractureTime = runtime.UltimateVisualTime();
        runtime.Cast(base, {0, 0}, true, 1.f);
        Require(std::abs(runtime.UltimateVisualTime() - fractureTime) < .001f &&
                    !runtime.UltimateVisualFaceLeft(),
                "A following skill must not restart or mirror the existing screen fracture");
        runtime.Update(.45f, session, map);
        Require(runtime.UltimateVisualTime() >= 1.89f, "Screen fracture did not expire");
        runtime.Reset();
        Require(!runtime.Casting() && runtime.Shots().empty() &&
                    runtime.UltimateVisualTime() >= 1.89f,
                "Ultimate reset failed");
        std::cout << "기사 스킬 검증 통과: 습득·MP·쿨타임·패시브·시간차·수명·범위·관통\n";
        return 0;
    }
    catch (const std::exception& error)
    {
        std::cerr << "기사 스킬 검증 실패: " << error.what() << '\n';
        return 1;
    }
}
