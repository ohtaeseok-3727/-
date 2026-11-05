#include "stdafx.h"
#include "GameSession.h"
#include "Chapter1.h"
#include <iostream>
#include <limits>
#include <stdexcept>

void VerifyPersistenceAndTrading();
void VerifyUiControls();

namespace
{
    void Require(bool condition, const char* message)
    {
        if (!condition)
        {
            throw std::runtime_error(message);
        }
    }

    // Test-only chapter: never registered in the playable catalog.
    class FixtureChapter final : public Chapter
    {
      public:
        int entries = 0;
        int enemyCount = 1;
        int enemyHealth = LevelTuning::EnemyHealth;

        const char* Title() const override
        {
            return "검증용 챕터";
        }

        MapId InitialMap() const override
        {
            return 7;
        }

        MapDefinition BuildMap(MapId id) const override
        {
            if (id != 0 && id != 7)
            {
                throw std::invalid_argument("Invalid fixture map");
            }
            MapDefinition map;
            map.id = id;
            map.title = "검증용 지역";
            map.areas = {{MapRegion::Village, "검증용 구역", {0.f, 0.f}, 120.f, 120.f}};
            map.roads = {{{-100.f, 0.f}, {100.f, 0.f}, 100.f}};
            return map;
        }

        void Reset() override
        {
            entries = 0;
            ClearTravel();
        }

        void EnterMap(MapId) noexcept override
        {
            ++entries;
        }

        void Update(float,
                    WorldPosition,
                    bool,
                    const LevelMap&,
                    EnemySystem& enemies,
                    PlayerProgression&) override
        {
            if (enemies.Enemies().empty())
            {
                EnemyStats stats;
                stats.health = enemyHealth;
                for (int i = 0; i < enemyCount; ++i)
                {
                    enemies.Add(
                        {40.f + static_cast<float>(i) * 5.f, 0.f}, 1, EnemyRole::Melee, stats);
                }
            }
        }

        std::string Interact(const MapLandmark& site,
                             const LevelMap&,
                             EnemySystem&,
                             PlayerProgression&,
                             PlayerInventory& inventory) override
        {
            if (site.object == 1)
            {
                inventory.AddGold(23);
                inventory.AddEquipment(0, 4);
                RequestTravel({{}, 7, std::nullopt});
            }
            else
            {
                RequestTravel({"missing", std::nullopt, std::nullopt});
            }
            return "검증 이동";
        }

        void Retry(EnemySystem& enemies) override
        {
            ClearTravel();
            enemies.Clear();
        }

        bool ControlLocked() const override
        {
            return false;
        }

        bool Complete() const override
        {
            return false;
        }

        const std::string& Message() const override
        {
            return m_Message;
        }

        std::string Objective() const override
        {
            return m_Message;
        }

        void DrawEnemy(Renderer&, const FieldEnemy&, Point) const override
        {
        }

      private:
        std::string m_Message = "검증 중";
    };

    void VerifyManaRecovery()
    {
        for (int profession = 0; profession < 2; ++profession)
        {
            PlayerProgression progress;
            progress.Reset(profession);
            progress.SpendMana(progress.MaxMana());
            for (int tick = 0; tick < 32; ++tick)
            {
                progress.UpdateManaRecovery(.125f);
            }
            Require(progress.Mana() == progress.MaxMana() * 4 / 100,
                    "Sub-point natural recovery was lost between frames");
            progress.UpdateManaRecovery(1000.f);
            Require(progress.Mana() == progress.MaxMana(), "Natural recovery exceeded max mana");
            progress.SpendMana(progress.MaxMana());
            progress.UpdateManaRecovery(.125f);
            Require(progress.Mana() == 0, "Full mana banked excess natural recovery");
            progress.RecoverMana(.75);
            progress.Restore();
            progress.SpendMana(progress.MaxMana());
            progress.RecoverMana(.5);
            Require(progress.Mana() == 0, "Full restoration retained fractional recovery");
            progress.DebugSetLevel(2);
            progress.SpendMana(progress.MaxMana());
            for (int hit = 0; hit < 4; ++hit)
            {
                progress.RecoverMana(progress.MaxMana() * .05);
            }
            Require(progress.Mana() == progress.MaxMana() / 5,
                    "Percentage recovery lost fractional mana at higher levels");
            const int beforeInvalidTime = progress.Mana();
            progress.UpdateManaRecovery(0.f);
            progress.UpdateManaRecovery(-1.f);
            progress.UpdateManaRecovery(std::numeric_limits<float>::quiet_NaN());
            progress.UpdateManaRecovery(std::numeric_limits<float>::infinity());
            Require(progress.Mana() == beforeInvalidTime, "Invalid time changed mana");

            LevelMap map;
            map.Generate(743);
            GameSession session;
            session.Reset(profession, map.Seed(), map);
            session.RegisterChapter("mana-fixture",
                                    []
                                    {
                                        auto chapter = std::make_unique<FixtureChapter>();
                                        chapter->enemyCount = 2;
                                        chapter->enemyHealth = 1000;
                                        return chapter;
                                    });
            WorldPosition destination;
            Require(session.TravelTo({"mana-fixture", 0, std::nullopt}, map, destination),
                    "Mana recovery fixture could not be entered");
            session.Update(0.f, {0.f, 0.f}, map);
            auto& player = session.Progress();
            const int maximum = player.MaxMana();
            player.SpendMana(maximum);
            session.Strike(1, {100.f, 0.f}, false, map);
            Require(player.Mana() == 0, "A missed basic attack restored mana");
            session.Strike(1, {0.f, 0.f}, false, map);
            Require(player.Mana() == maximum * 5 / 100 && session.Kills() == 0,
                    "First hit must restore 5% once even against multiple surviving enemies");
            session.Strike(2, {0.f, 0.f}, false, map);
            Require(player.Mana() == maximum * 10 / 100, "Second hit did not restore 5%");
            session.Strike(3, {0.f, 0.f}, false, map);
            Require(player.Mana() == maximum * 40 / 100, "Final hit did not restore 30%");
            session.Update(1.25f, {0.f, 0.f}, map);
            session.Update(1.25f, {0.f, 0.f}, map);
            Require(player.Mana() == maximum * 40 / 100 + maximum / 40,
                    "Session update did not regenerate 1% of max mana per second");
            const int beforeSpell = player.Mana();
            session.StrikeArea({40.f, 0.f}, 100.f, 2000);
            Require(player.Mana() == beforeSpell, "A skill hit restored basic-attack mana");
            session.Strike(1, {0.f, 0.f}, false, map);
            Require(player.Mana() == beforeSpell, "Striking a corpse restored mana");
            player.Damage(player.MaxHealth());
            session.Update(10.f, {0.f, 0.f}, map);
            Require(player.Mana() == beforeSpell, "Mana regenerated after defeat");
        }
        std::cout << "PASS mana regeneration / combo hits / misses / multi-target / cap / defeat\n";
    }
}

void VerifyChapterTransitions()
{
    VerifyPersistenceAndTrading();
    VerifyUiControls();
    VerifyManaRecovery();
    GameSession session;
    LevelMap map;
    map.Generate(743);
    session.Reset(0, map.Seed(), map);
    session.RegisterChapter("fixture",
                            []
                            {
                                return std::make_unique<FixtureChapter>();
                            });
    bool duplicateRejected = false;
    try
    {
        session.RegisterChapter("fixture",
                                []
                                {
                                    return std::make_unique<FixtureChapter>();
                                });
    }
    catch (const std::invalid_argument&)
    {
        duplicateRejected = true;
    }
    Require(duplicateRejected, "Duplicate registration was accepted");
    Require(session.FindChapter("fixture") == nullptr, "Chapter was instantiated eagerly");

    // Record actual C01 progress, not just a synthetic state variable.
    session.Inspect(map.Layout().Landmarks().front(), map);
    const auto* first = dynamic_cast<const Chapter1*>(session.FindChapter("chapter1"));
    Require(first && first->State().rationInteractionComplete, "C01 progress fixture failed");
    session.Progress().DebugSetLevel(20);
    session.Progress().AddExperience(10);
    session.Progress().Damage(12);
    session.Progress().SpendMana(3);
    const int health = session.Progress().Health();
    const int mana = session.Progress().Mana();
    const int experience = session.Progress().Experience();
    WorldPosition destination;
    const auto revision = map.Revision();
    Require(session.TravelTo({"fixture", 0, std::nullopt}, map, destination),
            "Cross-chapter travel failed");
    Require(map.Map() == 0 && map.ChapterId() == "fixture" && map.Revision() > revision,
            "Same local map ID did not replace the chapter map");
    auto& fixture = dynamic_cast<const FixtureChapter&>(session.CurrentChapter());
    Require(fixture.entries == 1, "Chapter entry was not committed exactly once");
    session.Update(.01f, {0.f, 0.f}, map);
    Require(!session.Enemies().Enemies().empty(), "Enemy fixture was not spawned");
    const auto teleportRevision = map.Revision();
    Require(session.TravelTo({{}, 0, WorldPosition{10.f, 0.f}}, map, destination),
            "Teleport failed");
    Require(map.Revision() == teleportRevision && fixture.entries == 1 &&
                !session.Enemies().Enemies().empty() && destination.x == 10.f,
            "Same-map teleport reset encounters or map revision");

    MapLandmark exit{LandmarkKind::Sign, {0.f, 0.f}, "검증", "검증", 0.f, 1};
    session.Inspect(exit, map);
    const int gold = session.Inventory().Gold();
    const auto items = session.Inventory().Items().size();
    const int equipment = session.Inventory().EquippedIndex();
    Require(session.ApplyPendingTravel(map, destination) && map.Map() == 7,
            "Queued map travel failed");
    Require(session.Enemies().Enemies().empty() && !session.HasLandmarkInteraction(),
            "Map-local actors or interaction text survived map replacement");
    Require(!session.ApplyPendingTravel(map, destination), "Travel request executed twice");
    Require(session.TravelTo({"chapter1", 0, std::nullopt}, map, destination),
            "Return travel failed");
    Require(session.FindChapter("chapter1") == first && first->State().rationInteractionComplete,
            "Returning to a chapter lost its progress");
    Require(session.Progress().Level() == 20 && session.Progress().Health() == health &&
                session.Progress().Mana() == mana &&
                session.Progress().Experience() == experience &&
                session.Inventory().Gold() == gold && session.Inventory().Items().size() == items &&
                session.Inventory().EquippedIndex() == equipment,
            "Chapter travel reset persistent player data");

    const auto unchanged = map.Revision();
    const auto previousDestination = destination;
    Require(!session.TravelTo({"missing", 0, std::nullopt}, map, destination),
            "Unknown chapter accepted");
    Require(!session.TravelTo({"fixture", 999, std::nullopt}, map, destination),
            "Unknown map accepted");
    Require(!session.TravelTo({"fixture", 0, WorldPosition{5000.f, 5000.f}}, map, destination),
            "Blocked destination accepted");
    Require(!session.TravelTo({{}, 0, WorldPosition{std::numeric_limits<float>::quiet_NaN(), 0.f}},
                              map,
                              destination),
            "Non-finite destination accepted");
    Require(session.ActiveChapterId() == "chapter1" && map.Revision() == unchanged &&
                destination.x == previousDestination.x && destination.y == previousDestination.y,
            "Rejected travel changed the current world");
    Require(session.TravelTo({"fixture", std::nullopt, std::nullopt}, map, destination) &&
                map.Map() == 7,
            "Chapter-authored initial map was ignored");
    exit.object = 2;
    session.Inspect(exit, map);
    Require(!session.ApplyPendingTravel(map, destination) &&
                !session.CurrentChapter().PendingTravel(),
            "Rejected request was not consumed");

    EnemySystem enemies;
    enemies.Add({35.f, 0.f}, 1, EnemyRole::Boss);
    enemies.Strike({0.f, 0.f}, false, 1000, map);
    Require(enemies.Enemies().front().health == 0, "Generic boss inherited Gruma immortality");
    EnemyStats scripted;
    scripted.minimumHealth = 1;
    enemies.Clear();
    enemies.Add({35.f, 0.f}, 1, EnemyRole::Boss, scripted);
    enemies.Strike({0.f, 0.f}, false, 1000, map);
    Require(enemies.Enemies().front().health == 1, "Authored scripted health floor was ignored");

    session.Reset(0, map.Seed(), map);
    Require(session.ActiveChapterId() == "chapter1" && session.Progress().Level() == 1 &&
                session.Inventory().Gold() == 0 && session.FindChapter("fixture") == nullptr &&
                !dynamic_cast<const Chapter1&>(session.CurrentChapter())
                     .State()
                     .rationInteractionComplete,
            "New game did not reset chapter and player progress");
    std::cout
        << "PASS chapter registration / travel / persistence / rejection / boss policy / new game\n";
}
