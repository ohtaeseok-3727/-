#include "stdafx.h"
#include "GameSession.h"
#include "Chapter1.h"
#include "Chapter2.h"
#include <cmath>
#include <chrono>
#include <algorithm>
#include <iostream>
#include <stdexcept>

namespace
{
    void Require(bool value, const char* message)
    {
        if (!value)
        {
            throw std::runtime_error(message);
        }
    }

    void VerifyFieldPopulation()
    {
        MapDefinition definition;
        definition.openWorld = true;
        definition.areas.push_back({MapRegion::Woods, "field", {0, -1400}, 1000.f, 700.f});
        definition.roads.push_back({{0, -700}, {0, -2100}, 100.f});
        LevelMap map;
        map.Load("field", definition);
        EnemySystem enemies;
        FieldPopulation population;
        population.Populate(map, enemies);
        Require(!enemies.Enemies().empty() && enemies.Enemies().size() <= 36,
                "Outdoor population must create bounded packs");
        const auto count = enemies.Enemies().size();
        const auto first = enemies.Enemies().front().position;
        for (const auto& enemy : enemies.Enemies())
        {
            Require(enemy.encounter >= 100000 && !map.Blocked(enemy.position.x, enemy.position.y),
                    "Field pack overlaps story IDs or blocked terrain");
        }
        population.Update(50.f, first, enemies);
        Require(enemies.Enemies().size() == count, "Living packs were duplicated");
        enemies.Clear();
        population.Update(44.f, first, enemies);
        Require(enemies.Enemies().empty(), "Field pack respawned before cooldown");
        population.Update(2.f, {first.x + 1100.f, first.y}, enemies);
        Require(!enemies.Enemies().empty() && enemies.Enemies().size() <= 36,
                "Distant defeated pack did not respawn within its cap");
    }

    void VerifyCombatGuidance()
    {
        LevelMap map;
        map.Generate(743);
        GameSession session;
        session.Reset(1, 743, map);
        session.OpenWorld(map);
        const auto ration = session.TrackedGoal();
        Require(ration && ration->hint == "[Space] 확인", "Initial interaction hint missing");
        const auto* site = map.Layout().NearbyLandmark(ration->position);
        Require(site && site->object == Chapter1Objects::Ration, "Ration target is not selectable");
        session.Inspect(*site, map);
        const auto raid = session.TrackedGoal();
        Require(raid && raid->hint.find("Ctrl") != std::string::npos &&
                    std::abs(raid->position.y + 920.f) < .1f,
                "Farm raid guide points at the investigation instead of combat");
        Chapter2 chapter;
        MapLandmark command{
            LandmarkKind::Sign, {}, "지휘 인장", "", 0.f, Chapter2Objects::CommandTarget};
        Require(chapter.InteractionPrompt(command).find("Space") == std::string::npos,
                "Protected command target incorrectly advertises Space interaction");
        MapLandmark mural{LandmarkKind::Sign, {}, "벽화", "", 0.f, Chapter2Objects::Mural};
        Require(chapter.InteractionPrompt(mural).find("Space 유지") != std::string::npos,
                "Mural hold hint missing");
        session.Reset(1, 743, map);
        session.OpenWorld(map, "chapter2");
        Require(!session.Enemies().Enemies().empty(), "Chapter2 field population missing");
        for (const auto& enemy : session.Enemies().Enemies())
        {
            Require(enemy.maxHealth == LevelTuning::OrcHealth,
                    "Field orc health differs from authored common orcs");
        }
        MapDefinition field;
        field.openWorld = true;
        field.roads.push_back({{-1000.f, 0.f}, {1000.f, 0.f}, 300.f});
        map.Load("cue-test", field);
        EnemyStateMachine ai;
        std::vector<FieldEnemy> others;
        for (const auto role : {EnemyRole::Melee, EnemyRole::Ranged})
        {
            for (bool inside : {false, true})
            {
                FieldEnemy enemy;
                enemy.position = {};
                enemy.role = role;
                enemy.ai.state = EnemyState::Windup;
                enemy.windup = .01f;
                int budget = 2;
                const WorldPosition player{enemy.AttackRange() + (inside ? -1.f : 1.f), 0.f};
                const int damage = ai.Update(enemy, others, .02f, player, map, true, budget);
                Require((damage > 0) == inside,
                        "Attack range used by warning and hit resolution diverged");
            }
        }
    }

    void VerifyOrcDeathAnimation()
    {
        LevelMap map;
        MapDefinition definition;
        definition.openWorld = true;
        map.Load("death-test", definition);
        Chapter2 chapter;
        for (bool preserve : {false, true})
        {
            for (bool scripted : {false, true})
            {
                EnemySystem enemies;
                EnemyStats stats;
                stats.preserveCorpse = preserve;
                stats.scriptedAI = scripted;
                enemies.Add({0, 0}, 100000, EnemyRole::Melee, stats);
                const auto id = enemies.Enemies().front().id;
                enemies.Update(2.f, {5000, 5000}, map);
                enemies.DamageEnemy(id, 10000);
                const int damage = enemies.Update(.2f, {5000, 5000}, map, 1500.f);
                const auto dead = enemies.Enemies().front();
                Require(dead.ai.state == EnemyState::Dead && dead.stateTime > .19f &&
                            dead.stateTime < .21f && damage == 0,
                        "Orc death animation timer stopped or inherited the previous state time");
                Require(chapter.EnemyVisible(dead), "Dying field orc disappeared before animation");
                enemies.Add({100, 0}, 100001);
                Require(enemies.Enemies().size() == 2 && enemies.Enemies().front().id == id,
                        "Dying orc slot was reused before the animation finished");
                enemies.Update(1.f, {5000, 5000}, map, 1500.f);
                const float finished = enemies.Enemies().front().stateTime;
                Require(std::abs(finished - .8f) < .001f &&
                            chapter.EnemyVisible(enemies.Enemies().front()) == preserve,
                        "Death animation did not finish with the correct corpse visibility");
                enemies.Update(10.f, {5000, 5000}, map, 1500.f);
                Require(enemies.Enemies().front().stateTime == finished,
                        "Finished corpse continued updating animation time");
                enemies.Add({200, 0}, 100002);
                Require(enemies.Enemies().size() == (preserve ? 3u : 2u),
                        "Finished corpse slot reuse policy changed");
            }
        }
    }

    void VerifyPatrol()
    {
        MapDefinition definition;
        definition.openWorld = true;
        definition.roads.push_back({{-1000, 0}, {1000, 0}, 200.f});
        LevelMap map;
        map.Load("patrol-test", definition);
        EnemySystem enemies;
        enemies.Add({0, 0}, 100000);
        for (int i = 0; i < 120; ++i)
        {
            enemies.Update(1.f / 60.f, {700, 0}, map);
        }
        auto enemy = enemies.Enemies().front();
        Require(enemy.ai.state == EnemyState::Patrol && !enemy.ai.alerted &&
                    std::hypot(enemy.position.x, enemy.position.y) > 1.f &&
                    std::hypot(enemy.position.x, enemy.position.y) <= 81.f,
                "Idle enemy must patrol locally without detecting a distant player");
        enemies.Update(.21f, {enemy.position.x + 120.f, enemy.position.y}, map);
        Require(enemies.Enemies().front().ai.alerted &&
                    enemies.Enemies().front().ai.state == EnemyState::Chase,
                "Nearby visible player did not trigger chase");
        for (int i = 0; i < 600; ++i)
        {
            enemies.Update(1.f / 60.f, {1000, 0}, map);
        }
        enemy = enemies.Enemies().front();
        Require(!enemy.ai.alerted && !enemy.ai.returning && enemy.ai.state == EnemyState::Patrol &&
                    std::hypot(enemy.position.x, enemy.position.y) <= 81.f,
                "Lost target did not return to local patrol");
        definition.roads.clear();
        definition.roads.push_back({{-40, 0}, {40, 0}, 40.f});
        definition.roads.push_back({{120, 0}, {160, 0}, 40.f});
        definition.water.push_back({{80, 0}, 16.f, 200.f});
        map.Load("occluded-test", definition);
        enemies.Clear();
        enemies.Add({0, 0}, 100000);
        enemies.Update(.21f, {140, 0}, map);
        Require(!enemies.Enemies().front().ai.alerted,
                "Enemy detected player through blocked terrain");
    }

    void VerifyPropCache()
    {
        MapDefinition definition;
        definition.openWorld = true;
        LevelMap cached;
        cached.Load("cache-check", definition);
        cached.Generate(743);
        // Negative cells and coordinates 64 cells apart deliberately replace the same slots.
        for (unsigned seed : {743u, 91u})
        {
            cached.Generate(seed);
            LevelMap fresh;
            fresh.Load("cache-check", definition);
            fresh.Generate(seed);
            for (int chunk : {-17, -1, 0, 15, 16, -17})
            {
                const auto actual = cached.ChunkObjects(chunk, -1);
                fresh.Load("cache-check", definition);
                const auto expected = fresh.ChunkObjects(chunk, -1);
                Require(actual.size() == expected.size(), "Prop cache changed occupancy");
                for (std::size_t i = 0; i < actual.size(); ++i)
                {
                    Require(actual[i].x == expected[i].x && actual[i].y == expected[i].y &&
                                actual[i].kind == expected[i].kind &&
                                actual[i].size == expected[i].size,
                            "Prop cache changed deterministic geometry");
                    Require(cached.Blocked(actual[i].x, actual[i].y), "Cached prop lost collision");
                }
            }
        }
        definition.water.push_back({{0.f, 0.f}, 10000.f, 10000.f});
        cached.Load("cache-check", definition);
        Require(cached.ChunkObjects(-1, -1).empty(), "Map reload retained cached props");
    }

    const MapLandmark& Site(const LevelMap& map, int region, ObjectId object)
    {
        for (const auto& site : map.Layout().Landmarks())
        {
            if (site.worldRegion == region && site.object == object)
            {
                return site;
            }
        }
        throw std::runtime_error("Missing world landmark");
    }

    bool Same(WorldPosition a, WorldPosition b)
    {
        return std::abs(a.x - b.x) < .01f && std::abs(a.y - b.y) < .01f;
    }
}

int RunOpenWorldVerification()
{
    try
    {
        VerifyPropCache();
        VerifyFieldPopulation();
        VerifyPatrol();
        VerifyOrcDeathAnimation();
        VerifyCombatGuidance();
        LevelMap map;
        map.Generate(743);
        GameSession session;
        session.Reset(1, 743, map);
        session.OpenWorld(map);
        const auto revision = map.Revision();
        Require(map.Layout().Definition().openWorld && session.Atlas().Regions().size() == 9,
                "World must contain the nine authored regions");
        for (std::size_t i = 0; i < session.Atlas().Regions().size(); ++i)
        {
            const auto& region = session.Atlas().Regions()[i];
            const auto entry = session.Atlas().ToWorld(i, region.local.entry);
            Require(!map.Blocked(entry.x, entry.y), "World camp entry is blocked");
            Require(Same(session.Atlas().ToLocal(i, entry), region.local.entry),
                    "World/local coordinate round trip changed an anchor");
            Require(session.Atlas().RegionAt(entry) == i, "Region ownership at entry is ambiguous");
            session.FollowPlayer(entry);
            if (i == 0 || i == 1 || i == 3 || i == 4)
            {
                // Use the actual village outskirts, forest, Graywall and canyon.
                // A large synthetic field alone misses overly restrictive placement.
                MapLayout localLayout;
                localLayout.Load(region.local);
                std::size_t fieldCount = 0;
                for (const auto& enemy : session.Enemies().Enemies())
                {
                    if (enemy.encounter < 100000)
                    {
                        continue;
                    }
                    ++fieldCount;
                    const auto world = session.Atlas().ToWorld(i, enemy.position);
                    Require(!map.Blocked(world.x, world.y) &&
                                !localLayout.Interior(enemy.position) &&
                                localLayout.RegionAt(enemy.position) != MapRegion::Village &&
                                localLayout.OnRoad(enemy.position, 130.f),
                            "Authored field pack spawned in blocked or protected terrain");
                }
                Require(fieldCount > 0 && fieldCount <= 36,
                        "Authored outdoor region must automatically spawn bounded field packs");
            }
            Require(session.ActiveChapterId() == region.chapter,
                    "Walking did not activate chapter owner");
            Require(map.Revision() == revision, "Walking replaced the world map");
        }
        // Sample all roads, including the mine bypass and physical fortress return loop.
        for (const auto& road : map.Layout().Roads())
        {
            const float distance = std::hypot(road.to.x - road.from.x, road.to.y - road.from.y);
            const int steps = static_cast<int>(std::ceil(distance / 4.f)) + 1;
            for (int i = 0; i <= steps; ++i)
            {
                const float t = float(i) / steps;
                Require(!map.Blocked(road.from.x + (road.to.x - road.from.x) * t,
                                     road.from.y + (road.to.y - road.from.y) * t),
                        "A continuous world road is blocked");
            }
        }
        session.FollowPlayer({0, 0});
        session.Inspect(Site(map, 0, Chapter1Objects::Ration), map);
        for (int i = 0; i < 50; ++i)
        {
            session.Update(.1f, {0, 0}, map);
        }
        Require(!session.Enemies().Enemies().empty(), "Farm raid did not spawn");
        const auto enemy = session.Enemies().Enemies().front();
        session.FollowPlayer(session.Atlas().ToWorld(1, {0, 0}));
        auto raidGoal = Site(map, 0, Chapter1Objects::FoodEvidence).position;
        raidGoal.y += 110.f;
        Require(session.TrackedGoal() && Same(session.TrackedGoal()->position, raidGoal),
                "Exploration must not replace the unresolved story objective");
        session.FollowPlayer({0, 0});
        Require(session.Enemies().Enemies().front().id == enemy.id &&
                    session.Enemies().Enemies().front().health == enemy.health,
                "Revisiting reset a living encounter");
        const auto raid = Site(map, 0, Chapter1Objects::FoodEvidence).position;
        WorldPosition destination;
        Require(!session.ReturnToCamp(raid, destination), "Camp recall bypassed nearby enemies");
        session.Progress().Damage(session.Progress().MaxHealth());
        Require(session.IsDefeated(), "Lethal damage must wait for explicit retry");
        session.Update(.1f, raid, map);
        Require(session.IsDefeated(), "Update silently revived a defeated player");
        Require(session.ReturnToCamp(raid, destination) && Same(destination, session.Checkpoint()),
                "Retry did not return to registered camp");
        Require(session.Progress().Health() == session.Progress().MaxHealth() &&
                    session.Progress().Mana() == session.Progress().MaxMana(),
                "Retry must restore both health and mana");
        Require(session.Inventory().Gold() == 40, "Retry lost earned gold");
        Require(map.Revision() == revision, "Retry rebuilt the world map");
        session.Update(.1f, {0, 0}, map);
        session.Progress().DebugSetLevel(20);
        const auto position = session.Enemies().Enemies().front().position;
        session.Strike(3, position, false, map);
        Require(!session.Loot().Drops().empty(), "Defeated enemies did not leave rewards");
        const auto dropCount = session.Loot().Drops().size();
        const auto kills = session.Kills();
        session.FollowPlayer(session.Atlas().ToWorld(1, {0, 0}));
        session.Update(.1f, session.Atlas().ToWorld(1, {0, 0}), map);
        session.FollowPlayer({0, 0});
        Require(session.Loot().Drops().size() == dropCount && session.Kills() == kills,
                "Revisiting lost rewards or counted kills twice");
        Require(!session.TravelTo({"chapter2", std::nullopt, std::nullopt}, map, destination) &&
                    map.Revision() == revision,
                "Legacy travel replaced the continuous world");
        session.Reset(0, 743, map);
        session.OpenWorld(map);
        Require(session.Progress().Level() == 1 && session.Inventory().Gold() == 0 &&
                    session.Kills() == 0 && session.Atlas().Regions().front().discovered &&
                    !session.Atlas().Regions().back().discovered,
                "New game kept exploration or character progress from the previous session");

        Chapter2 chapter;
        chapter.Reset();
        chapter.SetContinuousWorld(true);
        chapter.VisitMap(Chapter2Maps::Canyon);
        chapter.VisitMap(Chapter2Maps::Graywall);
        Require(chapter.RecordEvent(Chapter2Event::BriefingHeard),
                "Exploration corrupted briefing state");
        Require(chapter.RecordEvent(Chapter2Event::SiegeMismatchFound),
                "Missing reconnaissance event");
        Require(chapter.RecordEvent(Chapter2Event::ScheduleRecovered), "Missing schedule event");
        Require(chapter.RecordEvent(Chapter2Event::FrontlineSupported), "Missing frontline event");
        Require(chapter.RecordEvent(Chapter2Event::BeaconDirectionFound), "Missing signal event");
        Require(chapter.RecordEvent(Chapter2Event::PursuitAbandoned), "Missing pursuit event");
        chapter.VisitMap(Chapter2Maps::Canyon);
        Require(chapter.RecordEvent(Chapter2Event::ConvoyPlanRead), "Missing canyon briefing");
        Require(chapter.ChooseTarget(Chapter2Target::Workshop) && !chapter.PendingTravel(),
                "Choosing an open-world objective must not teleport the player");
        chapter.VisitMap(Chapter2Maps::OldFortress);
        chapter.VisitMap(Chapter2Maps::Canyon);
        Require(chapter.State().activeTarget == Chapter2Target::Workshop,
                "Exploration lost the chosen objective");
        chapter.VisitMap(Chapter2Maps::OldFortress);
        Require(!chapter.BlocksMovement({1700, -1480}),
                "Exterior exploration hit an infinite story wall");
        std::cout
            << "PASS continuous world topology, revisits, objectives, retry and no-teleport choices\n";
        return 0;
    }
    catch (const std::exception& error)
    {
        std::cerr << "FAIL open world: " << error.what() << '\n';
        return 1;
    }
}

int RunEnemyBenchmark()
{
    LevelMap map;
    MapDefinition definition;
    definition.openWorld = true;
    map.Load("benchmark", definition);
    map.Generate(743);
    EnemySystem enemies;
    for (int y = -6; y < 6; ++y)
    {
        for (int x = -6; x < 6; ++x)
        {
            WorldPosition p{x * 110.f, y * 110.f};
            if (!map.Blocked(p.x, p.y))
            {
                enemies.Add(p, 100000);
            }
        }
    }
    std::vector<double> samples;
    for (int frame = 0; frame < 1800; ++frame)
    {
        const float angle = frame * .005f;
        const WorldPosition player{std::cos(angle) * 500.f, std::sin(angle) * 500.f};
        const auto start = std::chrono::steady_clock::now();
        enemies.Update(1.f / 60.f, player, map, 1500.f);
        const double ms =
            std::chrono::duration<double, std::milli>(std::chrono::steady_clock::now() - start)
                .count();
        if (frame >= 120)
        {
            samples.push_back(ms);
        }
    }
    double total = 0;
    for (double ms : samples)
    {
        total += ms;
    }
    std::sort(samples.begin(), samples.end());
    std::cout << "Enemy CPU benchmark: count=" << enemies.Enemies().size()
              << " mean_ms=" << total / samples.size()
              << " p95_ms=" << samples[samples.size() * 95 / 100] << std::endl;
    return 0;
}
