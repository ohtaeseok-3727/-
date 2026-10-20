#include "stdafx.h"
#include "Chapter3Verification.h"
#include "Chapter3.h"
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
        std::cout << "PASS 챕터 3 선행 조건·조사·비살상·재시도·분산 보존·정치 상태·월드 연결\n";
        return 0;
    }
    catch (const std::exception& error)
    {
        std::cerr << "FAIL 챕터 3: " << error.what() << '\n';
        return 1;
    }
}
