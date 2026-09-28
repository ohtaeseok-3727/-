#include "stdafx.h"
#include "Chapter1Verification.h"
#include "GameSession.h"
#include "Chapter1.h"
#include "Chapter2.h"
#include "BasicAttack.h"
#include "EnemyPathfinder.h"
#include <iostream>
#include <stdexcept>
#include <queue>
#include <set>
#include <cmath>

namespace
{
    void Require(bool condition, const char* message)
    {
        if (!condition)
        {
            throw std::runtime_error(message);
        }
    }

    class Run
    {
      public:
        LevelMap map;
        GameSession session;
        WorldPosition player{};

        Run(int profession)
        {
            map.Generate(743);
            session.Reset(profession, map.Seed(), map);
        }

        const Chapter1State& State() const
        {
            return dynamic_cast<const Chapter1&>(*session.FindChapter("chapter1")).State();
        }

        void Tick(float seconds = .1f, bool hold = false)
        {
            for (int i = 0; i < static_cast<int>(std::ceil(seconds / .05f)); ++i)
            {
                session.Update(.05f, player, map, hold);
                WorldPosition destination;
                if (session.ApplyPendingTravel(map, destination))
                {
                    player = destination;
                }
            }
        }

        void Interact(ObjectId object)
        {
            for (const auto& landmark : map.Layout().Landmarks())
            {
                if (landmark.object == object)
                {
                    player = landmark.position;
                    Require(!map.Blocked(player.x, player.y), "Interaction anchor is blocked");
                    session.Inspect(landmark, map);
                    return;
                }
            }
            throw std::runtime_error("Missing functional landmark");
        }

        void StrikeEnemy(std::size_t index)
        {
            const auto position = session.Enemies().Enemies()[index].position;
            player = {position.x - 35, position.y};
            if (map.Blocked(player.x, player.y))
            {
                player = {position.x + 35, position.y};
            }
            Require(map.ClearPath(player, position), "Enemy attack approach blocked");
            session.Strike(3, player, player.x > position.x, map);
        }

        void ClearEnemies(bool boss = false)
        {
            for (int round = 0; round < 100; ++round)
            {
                bool found = false;
                for (std::size_t i = 0; i < session.Enemies().Enemies().size(); ++i)
                {
                    const auto& enemy = session.Enemies().Enemies()[i];
                    if (enemy.health > 0 && (boss || enemy.role != EnemyRole::Boss))
                    {
                        found = true;
                        StrikeEnemy(i);
                    }
                }
                if (!found)
                {
                    return;
                }
            }
            throw std::runtime_error("Encounter cannot be cleared");
        }
    };

    void VerifyTopology()
    {
        for (const auto kind : {Chapter1Maps::Village,
                                Chapter1Maps::Forest,
                                Chapter1Maps::Mine,
                                Chapter1Maps::Chapter2Route})
        {
            LevelMap map;
            map.Generate(743);
            map.Load("chapter1", Chapter1{}.BuildMap(kind));
            constexpr int step = 20;
            std::queue<std::pair<int, int>> pending;
            std::set<std::pair<int, int>> visited;
            pending.push({0, 0});
            visited.insert({0, 0});
            while (!pending.empty())
            {
                const auto p = pending.front();
                pending.pop();
                for (const auto d : {std::pair<int, int>{1, 0}, {-1, 0}, {0, 1}, {0, -1}})
                {
                    const auto next = std::make_pair(p.first + d.first, p.second + d.second);
                    if (visited.count(next) || std::abs(next.first) > 100 ||
                        std::abs(next.second) > 200)
                    {
                        continue;
                    }
                    if (map.ClearPath({float(p.first * step), float(p.second * step)},
                                      {float(next.first * step), float(next.second * step)}))
                    {
                        visited.insert(next);
                        pending.push(next);
                    }
                }
            }
            for (const auto& site : map.Layout().Landmarks())
            {
                bool reached = false;
                for (const auto p : visited)
                {
                    const WorldPosition point{float(p.first * step), float(p.second * step)};
                    const float dx = point.x - site.position.x, dy = point.y - site.position.y;
                    if (dx * dx + dy * dy < 90 * 90 && map.ClearPath(point, site.position))
                    {
                        reached = true;
                        break;
                    }
                }
                if (!reached)
                {
                    std::cerr << "Unreachable: " << site.name << "\n";
                }
                Require(reached, "Map landmark unreachable from entry");
            }
            Require(map.Blocked(5000, 5000), "Map boundary can be bypassed");
        }
        std::cout << "PASS analytic map reachability / all functional anchors / boundaries\n";
    }

    void VerifyFlow(bool donation, bool drainage, int profession)
    {
        Run run(profession);
        run.Interact(Chapter1Objects::NorthExit);
        run.Tick();
        Require(run.map.Map() == Chapter1Maps::Village, "Village exit bypassed");
        run.Interact(Chapter1Objects::Ration);
        const int gold = run.session.Inventory().Gold();
        run.Interact(Chapter1Objects::Ration);
        Require(run.session.Inventory().Gold() == gold, "Payout duplicated");
        if (donation)
        {
            run.Interact(Chapter1Objects::Donation);
            Require(run.session.Inventory().Gold() == gold - 10, "Donation not charged");
        }
        run.Tick(4.2f);
        run.ClearEnemies();
        run.Tick();
        Require(run.State().farmRaidResolved, "Farm encounter did not resolve");
        run.Interact(Chapter1Objects::RepairEvidence);
        run.Interact(Chapter1Objects::FoodEvidence);
        run.Interact(Chapter1Objects::FoodEvidence);
        Require(run.State().investigationEvidenceCount == 2, "Duplicate evidence counted");
        run.Interact(Chapter1Objects::TransportEvidence);
        run.Interact(Chapter1Objects::NorthExit);
        run.Tick();
        Require(run.map.Map() == Chapter1Maps::Forest, "Forest transition failed");
        if (donation)
        {
            run.player = Chapter1::Position(run.map, Chapter1Objects::LightTracks);
            run.Tick();
            Require(run.State().wrongPathAmbushTriggered, "Wrong-path trigger failed");
            run.ClearEnemies();
        }
        for (auto object : {Chapter1Objects::HeavyTracks,
                            Chapter1Objects::Grain,
                            Chapter1Objects::Bark,
                            Chapter1Objects::Rope})
        {
            run.Interact(object);
        }
        Require(run.State().realTrackFound, "Tracking failed");
        run.Interact(Chapter1Objects::Ridge);
        run.Tick();
        run.Interact(Chapter1Objects::Prisoner);
        Require(!run.State().prisonersReleased, "Prisoner gate bypassed");
        run.Interact(Chapter1Objects::Wagon);
        run.ClearEnemies();
        run.Interact(Chapter1Objects::Prisoner);
        run.Interact(Chapter1Objects::Invoice);
        run.Interact(Chapter1Objects::MineExit);
        run.Tick();
        Require(run.map.Map() == Chapter1Maps::Mine, "Mine transition failed");
        run.Interact(drainage ? Chapter1Objects::Drain : Chapter1Objects::FrontGate);
        run.Tick();
        Require(run.State().mineEntryRoute == (drainage ? Chapter1MineEntryRoute::Drainage
                                                        : Chapter1MineEntryRoute::FrontGate),
                "Mine route wrong");
        if (!drainage)
        {
            run.ClearEnemies();
        }
        if (drainage)
        {
            for (auto object : {Chapter1Objects::ShiftBell,
                                Chapter1Objects::TransportRecords,
                                Chapter1Objects::CommandSeal})
            {
                run.Interact(object);
            }
            run.Interact(Chapter1Objects::Lift);
            run.Tick();
        }
        else
        {
            run.player = Chapter1::Arena();
            run.Tick();
        }
        Require(run.State().grumaDuelStarted, "Duel did not start");
        Require(run.session.Enemies().Enemies().size() == 1, "Duel contains additional enemies");
        for (int i = 0; i < 30 && !run.State().grumaLegInjured; ++i)
        {
            run.StrikeEnemy(0);
            run.Tick();
        }
        Require(run.State().grumaArmorBroken && run.State().grumaLegInjured,
                "Gruma damage thresholds missing");
        Require(run.State().phase == Chapter1Phase::ArmyCommand, "Army phase missing");
        Require(!run.State().heroArrived, "Hero arrived before player success");
        run.Interact(Chapter1Objects::ReinforcementGate);
        run.Tick(4.5f, true);
        Require(!run.State().strategicMissionSuccess, "Gate bypassed bells/support objectives");
        run.session.Progress().Heal(run.session.Progress().MaxHealth());
        run.ClearEnemies();
        for (auto object : {Chapter1Objects::BellA, Chapter1Objects::BellB, Chapter1Objects::BellC})
        {
            const auto pos = Chapter1::Position(run.map, object);
            run.player = {pos.x - 35, pos.y};
            for (int i = 0; i < 4; ++i)
            {
                run.session.Strike(3, run.player, false, run.map);
            }
        }
        Require(run.State().commandBellsDestroyed == 3, "Bell damage failed");
        run.Interact(Chapter1Objects::ReinforcementGate);
        run.Tick(1.5f, true);
        run.Tick(.1f, false);
        Require(run.State().gateProgress == 0, "Released hold was retained");
        run.session.Progress().Heal(run.session.Progress().MaxHealth());
        run.Tick(4.1f, true);
        Require(run.State().strategicMissionSuccess && !run.State().heroArrived,
                "Strategic-success ordering failed");
        run.Tick(5.2f);
        Require(run.State().heroArrived && run.State().playerIncapacitated,
                "Hero execution intercept failed");
        run.Tick(35.2f);
        Require(run.State().grumaDefeated && !run.session.IsDefeated(), "Payoff failed");
        Require(run.State().grumaArmorBroken && run.State().grumaLegInjured &&
                    run.State().reinforcementGateClosed,
                "Damage continuity lost");
        run.Interact(Chapter1Objects::Ledger);
        run.Interact(Chapter1Objects::Hero);
        run.Interact(Chapter1Objects::ChapterExit);
        run.Tick();
        Require(run.State().chapter1Complete && run.map.Map() == Chapter2Maps::Graywall,
                "C02 route not connected");
        Require(run.State().reliefDonationMade == donation, "Donation state changed");
        Require(run.State().triggers.test(35), "Exit trigger not recorded");
        std::cout << "PASS full C01 logic flow / " << (drainage ? "drain+lift" : "front+direct")
                  << " / profession=" << profession << "\n";
    }
}

int RunChapter1Verification()
{
    try
    {
        VerifyChapterTransitions();
        LevelMap pathMap;
        pathMap.Generate(743);
        pathMap.Load("chapter1", Chapter1{}.BuildMap(Chapter1Maps::Village));
        EnemyPathfinder pathfinder;
        std::vector<WorldPosition> path;
        const WorldPosition start{0.f, 0.f};
        const WorldPosition goal{0.f, -200.f};
        Require(pathMap.ClearPath(start, goal), "Direct-path fixture is blocked");
        Require(pathfinder.FindPath(start, goal, pathMap, path) && path.size() == 1 &&
                    path.front().x == goal.x && path.front().y == goal.y,
                "Open-ground path must go directly to the goal");
        Require(pathfinder.FindPath(start, start, pathMap, path) && path.size() == 1,
                "Coincident path endpoints failed");
        std::cout << "PASS direct enemy path and coincident endpoints\n";
        VerifyTopology();
        VerifyFlow(true, true, 0);
        VerifyFlow(false, false, 1);
        BasicAttack attack;
        attack.Press(1);
        attack.Press(1);
        attack.Press(1);
        int hits = 0;
        for (int i = 0; i < 1000; ++i)
        {
            attack.Update(.01f);
            hits += static_cast<int>(attack.ConsumeHits().size());
        }
        Require(hits == 3, "BasicAttack combo regression");
        std::cout << "PASS basic-attack 3-hit combo\n";
        std::cout
            << "PASS Chapter1 verification (logic/collision; visual gameplay requires separate review)\n";
        return 0;
    }
    catch (const std::exception& error)
    {
        std::cerr << "FAIL Chapter1 verification: " << error.what() << "\n";
        return 1;
    }
}
