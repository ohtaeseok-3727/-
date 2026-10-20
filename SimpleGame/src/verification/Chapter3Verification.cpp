#include "stdafx.h"
#include "Chapter3Verification.h"
#include "Chapter3.h"
#include "Chapter2.h"
#include "GameSession.h"
#include <algorithm>
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

    class ChapterFixture
    {
      public:
        Chapter3 chapter;
        LevelMap map;
        EnemySystem enemies;
        PlayerProgression progress;
        PlayerInventory inventory;

        ChapterFixture()
        {
            chapter.Reset();
            progress.Reset(0);
            inventory.Reset();
            map.Generate(743);
            Enter(Chapter3Maps::Memorial);
        }

        void Enter(MapId id)
        {
            map.Load("chapter3", chapter.BuildMap(id));
            chapter.EnterMap(id);
        }

        const MapLandmark& Site(ObjectId id) const
        {
            const auto& sites = map.Layout().Landmarks();
            const auto found = std::find_if(sites.begin(),
                                            sites.end(),
                                            [id](const auto& site)
                                            {
                                                return site.object == id;
                                            });
            Require(found != sites.end(), "Missing authored landmark");
            return *found;
        }

        std::string Inspect(ObjectId id)
        {
            return chapter.Interact(Site(id), map, enemies, progress, inventory);
        }

        void Tick(WorldPosition player, bool held = false, int frames = 1)
        {
            for (int i = 0; i < frames; ++i)
            {
                chapter.Update(.05f, player, held, map, enemies, progress);
            }
        }

        void FollowGoal(int frames)
        {
            const auto initial = chapter.Goal();
            Require(initial.has_value(), "Expected an initial goal");
            for (int i = 0; i < frames; ++i)
            {
                const auto goal = chapter.Goal();
                Require(goal.has_value(), "Expected an active goal");
                if (goal->map != initial->map || goal->object != initial->object ||
                    goal->label != initial->label)
                {
                    return;
                }
                Tick(goal->position.value_or(SitePosition(goal->object)), true);
            }
        }

        WorldPosition SitePosition(ObjectId id) const
        {
            if (id == 0)
            {
                return {};
            }
            return Site(id).position;
        }
    };

    void VerifyFlow()
    {
        using namespace Chapter3Objects;
        ChapterFixture f;
        f.Inspect(Ceremony);
        Require(f.chapter.Goal()->object == Ceremony, "Predecessor gate bypassed");
        f.chapter.ImportPrevious(
            {{"C02_COMPLETE", 1}, {"C02_SUPPLY_LOSS", 2}, {"C02_REFUGEE_LOSS", 1}});
        f.Inspect(Ceremony);
        f.Inspect(Plaque);
        f.Inspect(Rubbing);
        f.Enter(Chapter3Maps::Argen);
        f.Inspect(Permit);
        Require(f.chapter.Goal()->object == Petition, "Permit bypassed civic investigation");
        f.Inspect(Petition);
        Require(f.Inspect(Rations).find("보급 피해 단계 2") != std::string::npos,
                "Chapter 2 consequences not imported");
        f.Inspect(Permit);
        f.Inspect(Shortcut);
        f.FollowGoal(10);
        Require(f.chapter.Goal()->object == ManuscriptEarly, "Nonlethal chase did not finish");
        f.Inspect(ManuscriptLate);
        Require(f.chapter.Goal()->object == ManuscriptEarly, "Wrong chronology accepted");
        f.Inspect(ManuscriptEarly);
        f.Inspect(ManuscriptMiddle);
        f.Inspect(ManuscriptLate);
        f.Inspect(WorkshopLedger);
        Require(f.chapter.Goal()->object == FoundationWest, "Ledger bypassed physical restoration");
        for (int id = FoundationWest; id <= FoundationEast; ++id)
        {
            f.Tick(f.Site(id).position, true, 42);
        }
        f.Inspect(WorkshopLedger);
        f.Tick(f.Site(LastSermon).position);
        Require(f.chapter.Goal()->object == RefugeRoom, "Monastery route order bypassed");
        for (int id = RefugeRoom; id <= LastSermon; ++id)
        {
            f.Tick(f.Site(id).position);
        }
        f.Inspect(Censure);
        f.Inspect(TransferSeal);
        Require(f.chapter.Goal()->object == Edric, "Edric accountability skipped");
        f.Inspect(Edric);
        f.Inspect(TransferSeal);
        f.Enter(Chapter3Maps::Archive);
        f.Inspect(OriginalBox);
        Require(f.chapter.Goal()->object == MovementList, "Archive evidence requirements bypassed");
        f.Inspect(MovementList);
        f.Inspect(StorageDate);
        f.Inspect(WrongDateBox);
        f.Inspect(WrongSupplyBox);
        Require(f.chapter.ExportFlags().at("C03_PREWAR_EXPEDITION") == 0,
                "Decoy treated as original");
        f.Inspect(OriginalBox);
        f.Tick({0, -1100});
        Require(f.enemies.Boss() && f.enemies.Boss()->stats.minimumHealth == 1,
                "Serad does not have nonlethal health protection");
        f.enemies.StrikeArea(f.enemies.Boss()->position, 200.f, 100000);
        Require(f.enemies.Boss()->health == 1 && f.enemies.ConsumeScriptDefeats().empty(),
                "Ultimate killed Serad or awarded a kill");
        f.chapter.Retry(f.enemies);
        Require(f.chapter.ExportFlags().at("C03_PREWAR_EXPEDITION") == 1,
                "Retry lost proven evidence");
        f.FollowGoal(30);
        Require(f.chapter.Goal()->object == Brake, "Moving cart interaction failed");
        f.Tick(f.Site(Brake).position, true, 42);
        auto* boss = f.enemies.Boss();
        Require(boss != nullptr, "Serad was not recreated after retry");
        f.enemies.DamageEnemy(boss->id, 100000);
        const auto position = boss->position;
        f.Tick(position, true, 22);
        Require(f.chapter.ExportFlags().at("C03_SERAD_SURVIVED") == 1, "Nonlethal disarm failed");
        f.FollowGoal(22);
        Require(f.chapter.ExportFlags().at("C03_ORIGINAL_RECOVERED") == 1,
                "Stopped cart original not recovered");
        f.Enter(Chapter3Maps::Argen);
        f.Inspect(CopyCommunity);
        Require(f.chapter.ExportFlags().at("C03_DISTRIBUTED_COPIES") == 0,
                "Copies made without checking public claims");
        f.Inspect(ConfirmedNotice);
        f.Inspect(OpenNotice);
        f.Inspect(CopyCommunity);
        f.Inspect(CopySurvivors);
        f.Inspect(CopyArchive);
        f.Inspect(ExtraGuild);
        f.Inspect(ExtraClergy);
        Require(f.chapter.ExportFlags().at("C03_EXTRA_CUSTODIAN") == 2,
                "Repeated choice rewrote the extra custodian");
        f.Inspect(Testimony);
        Require(f.chapter.ExportFlags().at("C03_HERO_TESTIMONY") == 0,
                "Testimony skipped courier departures");
        f.Tick({0, -1900}, false, 82);
        f.Inspect(Testimony);
        f.Inspect(FieldPermit);
        Require(!f.chapter.Complete(), "Hearing and restrictions skipped");
        f.Inspect(Hearing);
        for (int id = RestrictedDoor; id <= CommandSeal; ++id)
        {
            f.Inspect(id);
        }
        const int gold = f.inventory.Gold();
        f.Inspect(SupplyDesk);
        Require(gold == 10 && f.inventory.Gold() == gold, "Restricted supplies repeated");
        f.Inspect(FieldPermit);
        const auto flags = f.chapter.ExportFlags();
        Require(f.chapter.Complete() && flags.at("C03_LIMITED_FIELD_PERMIT") == 1 &&
                    flags.at("C03_HERO_COMMAND_RETAINED") == 1 && flags.at("C03_WANTED") == 0 &&
                    flags.at("C03_WAR_ORIGIN_CONFIRMED") == 0,
                "Final political state or epistemic limits changed");
    }

    void VerifySkipping()
    {
        GameSession session;
        LevelMap map;
        map.Generate(743);
        session.Reset(0, 743, map);
        session.OpenWorld(map);
        WorldPosition destination{123.f, 456.f};
        Require(!session.CanSkipChapter2() && !session.CanSkipChapter3() &&
                    !session.SkipChapter3(destination) && destination.x == 123.f &&
                    destination.y == 456.f,
                "Skip bypassed predecessor requirements or moved on failure");
        Require(session.SkipChapter1(destination) && session.CanSkipChapter2(),
                "Chapter 1 skip did not unlock Chapter 2 skip");
        Require(session.SkipChapter2FieldStory(destination), "Field skip regressed");
        auto& second = dynamic_cast<Chapter2&>(session.CurrentChapter());
        Require(second.SelectRoute(Chapter2Route::Breach), "Could not select an existing route");
        const auto priorities = second.State().priority;
        const int refugeeLoss = second.State().outcome.refugeeLoss;
        const int gold = session.Inventory().Gold();
        const auto equipment = session.Inventory().Items().size();
        const int level = session.Progress().Level();
        const int experience = session.Progress().Experience();
        const int kills = session.Kills();
        session.Progress().Damage(10);
        Require(session.SkipChapter2(destination) && second.Complete() &&
                    second.State().priority == priorities &&
                    second.State().route == Chapter2Route::Breach &&
                    second.State().outcome.refugeeLoss == refugeeLoss &&
                    session.ActiveChapterId() == "chapter3" && session.CanSkipChapter3(),
                "Full Chapter 2 skip lost choices or failed to unlock Chapter 3");
        Require(session.SkipChapter3(destination), "Chapter 3 skip failed");
        const auto flags = session.FindChapter("chapter3")->ExportFlags();
        const auto city = session.Atlas().Find("chapter3", Chapter3Maps::Argen);
        Require(city && session.Atlas().RegionAt(destination) == *city &&
                    session.Checkpoint().x == destination.x &&
                    session.Checkpoint().y == destination.y && flags.at("C03_COMPLETE") == 1 &&
                    flags.at("C03_SERAD_SURVIVED") == 1 &&
                    flags.at("C03_DISTRIBUTED_COPIES") == 3 && flags.at("C03_WANTED") == 0 &&
                    flags.at("C03_WAR_ORIGIN_CONFIRMED") == 0,
                "Chapter 3 skip destination or ending flags are inconsistent");
        Require(session.Inventory().Gold() == gold &&
                    session.Inventory().Items().size() == equipment &&
                    session.Progress().Level() == level &&
                    session.Progress().Experience() == experience && session.Kills() == kills &&
                    session.Progress().Health() == session.Progress().MaxHealth() &&
                    session.Progress().Mana() == session.Progress().MaxMana(),
                "Skip awarded rewards, altered equipment, or failed to restore resources");
        Require(!session.CanSkipChapter2() && !session.CanSkipChapter3() &&
                    !session.SkipChapter3(destination),
                "Completed chapter skip can be repeated");
        for (const auto& site : map.Layout().Landmarks())
        {
            if (city && site.worldRegion == static_cast<int>(*city) &&
                site.object == Chapter3Objects::SupplyDesk)
            {
                session.Inspect(site, map);
            }
        }
        Require(session.Inventory().Gold() == gold, "Skipped support could be claimed afterward");
        session.Reset(0, 743, map);
        session.OpenWorld(map, "chapter2");
        Require(session.CanSkipChapter2() && session.SkipChapter2(destination),
                "Chapter 2 preview cannot skip the full story");
        session.Reset(0, 743, map);
        session.OpenWorld(map, "chapter3");
        Require(!session.CanSkipChapter2() && session.CanSkipChapter3() &&
                    session.SkipChapter3(destination),
                "Chapter 3 preview skip did not use its predecessor fixture");
    }

    void VerifyWorld()
    {
        GameSession session;
        LevelMap map;
        map.Generate(743);
        session.Reset(0, 743, map);
        session.OpenWorld(map);
        const auto region = session.Atlas().Find("chapter3", Chapter3Maps::Memorial);
        Require(region.has_value(), "Chapter 3 is absent from the normal world");
        const auto entry = session.Atlas().ToWorld(*region, {0, 180});
        session.FollowPlayer(entry);
        const auto initialGoal = session.TrackedGoal();
        for (const auto& site : map.Layout().Landmarks())
        {
            if (site.worldRegion == static_cast<int>(*region) &&
                site.object == Chapter3Objects::Ceremony)
            {
                session.Inspect(site, map);
            }
        }
        Require(&session.StoryChapter() == session.FindChapter("chapter1") &&
                    session.FindChapter("chapter3")->Goal()->object == Chapter3Objects::Ceremony &&
                    initialGoal && session.TrackedGoal()->position.x == initialGoal->position.x,
                "Early exploration bypassed Chapter 2 or replaced the story target");
        session.Reset(0, 743, map);
        session.OpenWorld(map, "chapter3");
        Require(session.ActiveChapterId() == "chapter3" && session.Atlas().Regions().size() == 9,
                "Chapter 3 preview or region registration failed");
        Require(!session.CanSkipChapter1() && !session.CanSkipChapter2FieldStory(),
                "Preview exposes predecessor skip actions");
        Require(session.TrackedGoal().has_value() && session.Enemies().Enemies().empty(),
                "Chapter 3 goal missing or civic area spawned monsters");
        for (const auto& region : session.Atlas().Regions())
        {
            if (region.chapter != "chapter3")
            {
                continue;
            }
            LevelMap local;
            local.Generate(743);
            local.Load(region.chapter, region.local);
            EnemySystem enemies;
            FieldPopulation population;
            population.Populate(local, enemies);
            Require(enemies.Enemies().empty(), "Ambient enemies spawned in civic/stealth region");
            Require(!local.Blocked(region.local.entry.x, region.local.entry.y),
                    "Blocked map entry");
            for (const auto& site : region.local.landmarks)
            {
                Require(!local.Blocked(site.position.x, site.position.y), "Blocked story landmark");
            }
        }
    }
}

int RunChapter3Verification()
{
    try
    {
        VerifyFlow();
        VerifyWorld();
        VerifySkipping();
        std::cout << "PASS 챕터 3 선행 조건·조사·비살상·재시도·분산 보존·정치 상태·월드 연결\n";
        return 0;
    }
    catch (const std::exception& error)
    {
        std::cerr << "FAIL 챕터 3: " << error.what() << '\n';
        return 1;
    }
}
