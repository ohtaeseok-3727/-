#include "stdafx.h"
#include "GameSession.h"
#include "Chapter1.h"
#include <iostream>
#include <limits>
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

    // Test-only chapter: never registered in the playable catalog.
    class FixtureChapter final : public Chapter
    {
      public:
        int entries = 0;

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
                enemies.Add({40.f, 0.f}, 1);
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
}

void VerifyChapterTransitions()
{
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
