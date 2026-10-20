#include "stdafx.h"
#include "GameSession.h"
#include "Chapter1.h"
#include "Chapter2.h"
#include "Chapter3.h"
#include "Renderer.h"
#include "SceneGraph.h"
#include <cmath>
#include <utility>

void GameSession::OpenWorld(LevelMap& map, const std::string& startChapter)
{
    if (m_OpenWorld)
    {
        return;
    }
    m_OpenWorld = false;
    m_StartAtChapter2 = startChapter == "chapter2";
    m_StartAtChapter3 = startChapter == "chapter3";
    m_Atlas.Clear();
    m_Regions.clear();
    for (auto& entry : m_Chapters)
    {
        auto& slot = entry.second;
        if (!slot.instance)
        {
            slot.instance = slot.factory();
            slot.instance->Reset();
        }
        slot.instance->SetContinuousWorld(true);
    }
    const auto add = [&](const char* chapter, MapId id, WorldPosition origin, WorldPosition exit)
    {
        m_Atlas.Add(chapter, m_Chapters.at(chapter).instance->BuildMap(id), origin, exit);
    };
    add("chapter1", Chapter1Maps::Village, {0, 0}, {0, -1430});
    add("chapter1", Chapter1Maps::Forest, {0, -2100}, {360, -1930});
    add("chapter1", Chapter1Maps::Mine, {360, -4650}, {0, -2960});
    add("chapter2", Chapter2Maps::Graywall, {360, -8650}, {0, -2250});
    add("chapter2", Chapter2Maps::Canyon, {360, -12200}, {0, -2050});
    add("chapter2", Chapter2Maps::OldFortress, {360, -15900}, {0, -3550});
    add("chapter3", Chapter3Maps::Memorial, {16000, -21000}, {0, -900});
    add("chapter3", Chapter3Maps::Argen, {4500, -23500}, {0, -2900});
    add("chapter3", Chapter3Maps::Archive, {16000, -28000}, {-850, -2000});
    add("chapter3", Chapter3Maps::ReadingRoom, {21000, -23500}, {0, 280});
    auto world = m_Atlas.Build();
    // An outdoor route connects both ends of the mine; exploration never requires the boss.
    world.roads.push_back({{360, -4030}, {1900, -4350}, 90.f});
    world.roads.push_back({{1900, -4350}, {1900, -8100}, 90.f});
    world.roads.push_back({{1900, -8100}, {360, -8650}, 90.f});
    // The fortress return trail is a physical loop back to the canyon relief camp.
    world.roads.push_back({{360, -19450}, {2100, -19800}, 90.f});
    world.roads.push_back({{2100, -19800}, {2100, -11600}, 90.f});
    world.roads.push_back({{2100, -11600}, {1010, -11750}, 90.f});
    world.roads.push_back({{1010, -11750}, {360, -11400}, 90.f});
    // A road from the relief camp reaches the record city without re-entering the fortress.
    world.roads.push_back({{1010, -11750}, {3000, -11750}, 100.f});
    world.roads.push_back({{3000, -11750}, {3000, -22850}, 100.f});
    world.roads.push_back({{3000, -22850}, {4500, -22850}, 100.f});
    world.roads.push_back({{4500, -22850}, {4500, -23300}, 100.f});
    map.Load("world", std::move(world));
    m_Regions.resize(m_Atlas.Regions().size());
    for (std::size_t i = 0; i < m_Regions.size(); ++i)
    {
        const auto& region = m_Atlas.Regions()[i];
        auto& runtime = m_Regions[i];
        runtime.map.Generate(map.Seed());
        runtime.map.Load(region.chapter, region.local);
        runtime.map.UseWorldGeometry(map, region.origin);
        runtime.enemies.SetIdentityBase((static_cast<std::uint64_t>(i) + 1) << 48);
        runtime.loot.Reset(map.Seed() + static_cast<unsigned>(i));
        runtime.population.Populate(runtime.map, runtime.enemies);
    }
    m_Region = m_StartAtChapter3 ? 6 : m_StartAtChapter2 ? 3 : 0;
    const auto& region = m_Atlas.Regions()[m_Region];
    m_ActiveChapterId = region.chapter;
    m_Chapter = m_Chapters.at(region.chapter).instance.get();
    SynchronizeChapterHistory();
    m_Chapter->VisitMap(region.local.id);
    m_Enemies = std::move(m_Regions[m_Region].enemies);
    m_Loot = std::move(m_Regions[m_Region].loot);
    m_Checkpoint = m_Atlas.ToWorld(m_Region, region.local.entry);
    m_Atlas.Discover(m_Region);
    m_OpenWorld = true;
    ShowNotice("변경에 도착했습니다. 금빛 목적지 표식을 따라가거나 M으로 세계 지도를 확인하세요.");
}

void GameSession::ActivateRegion(std::size_t region)
{
    if (region == m_Region)
    {
        return;
    }
    m_Regions[m_Region].enemies = std::move(m_Enemies);
    m_Regions[m_Region].loot = std::move(m_Loot);
    const auto& definition = m_Atlas.Regions()[region];
    m_Chapter->ClearTravel();
    m_Chapter = m_Chapters.at(definition.chapter).instance.get();
    m_ActiveChapterId = definition.chapter;
    SynchronizeChapterHistory();
    m_Chapter->VisitMap(definition.local.id);
    m_Enemies = std::move(m_Regions[region].enemies);
    m_Loot = std::move(m_Regions[region].loot);
    m_Region = region;
    m_Push = {};
    m_LandmarkInteractionTime = 0.f;
    m_LandmarkInteractionText.clear();
    if (m_Atlas.Discover(region))
    {
        ShowNotice("지역 발견 · " + definition.local.title +
                   " · 입구 야영지에서 휴식할 수 있습니다.");
    }
}

void GameSession::FollowPlayer(WorldPosition player)
{
    if (m_OpenWorld && !IsDefeated() && !m_Chapter->ControlLocked())
    {
        const auto region = m_Atlas.RegionAt(player);
        if (region != m_Region)
        {
            ActivateRegion(region);
            m_Chapter->PlayerArrived(LocalPlayer(player), m_Regions[m_Region].map);
        }
    }
}

bool GameSession::WorldEnabled() const
{
    return m_OpenWorld;
}

WorldPosition GameSession::LocalPlayer(WorldPosition world) const
{
    return m_OpenWorld ? m_Atlas.ToLocal(m_Region, world) : world;
}

WorldPosition GameSession::WorldOrigin() const
{
    return m_OpenWorld ? m_Atlas.Regions()[m_Region].origin : WorldPosition{};
}

bool GameSession::BlocksMovement(WorldPosition world) const
{
    return m_Chapter && m_Chapter->BlocksMovement(LocalPlayer(world));
}

const WorldAtlas& GameSession::Atlas() const
{
    return m_Atlas;
}

const Chapter& GameSession::StoryChapter() const
{
    if (!m_OpenWorld)
    {
        return CurrentChapter();
    }
    const auto* first = FindChapter("chapter1");
    if (m_StartAtChapter3)
    {
        return *FindChapter("chapter3");
    }
    if (!first->Complete() && !m_StartAtChapter2)
    {
        return *first;
    }
    const auto* second = FindChapter("chapter2");
    return second->Complete() ? *FindChapter("chapter3") : *second;
}

bool GameSession::StoryReady() const
{
    if (m_ActiveChapterId == "chapter3")
    {
        const auto* second = FindChapter("chapter2");
        return m_StartAtChapter3 || (second && second->Complete());
    }
    if (m_OpenWorld && m_ActiveChapterId == "chapter2")
    {
        return m_StartAtChapter2 || FindChapter("chapter1")->Complete();
    }
    return true;
}

void GameSession::SynchronizeChapterHistory()
{
    if (m_ActiveChapterId != "chapter3")
    {
        return;
    }
    ChapterFlags flags;
    if (const auto* second = FindChapter("chapter2"))
    {
        flags = second->ExportFlags();
    }
    if (m_StartAtChapter3)
    {
        // Preview supplies a declared predecessor fixture, never skips Chapter 3 objectives.
        flags = {{"C02_COMPLETE", 1}, {"C02_SUPPLY_LOSS", 1}, {"C02_REFUGEE_LOSS", 1}};
    }
    m_Chapter->ImportPrevious(flags);
}

std::string GameSession::LandmarkPrompt(const MapLandmark& site) const
{
    if (site.object == -1 && m_OpenWorld)
    {
        return "[Space] " + site.name + " · 회복/복귀 등록";
    }
    const auto* chapter = m_Chapter;
    if (m_OpenWorld && site.worldRegion >= 0 &&
        static_cast<std::size_t>(site.worldRegion) < m_Atlas.Regions().size())
    {
        chapter = FindChapter(m_Atlas.Regions()[site.worldRegion].chapter);
    }
    return chapter ? chapter->InteractionPrompt(site) : "[Space] " + site.name;
}

std::optional<WorldMarker> GameSession::TrackedGoal() const
{
    if (!m_OpenWorld)
    {
        return std::nullopt;
    }
    const auto& chapter = StoryChapter();
    const auto goal = chapter.Goal();
    if (!goal)
    {
        return std::nullopt;
    }
    const char* id = &chapter == FindChapter("chapter1")   ? "chapter1"
                     : &chapter == FindChapter("chapter2") ? "chapter2"
                                                           : "chapter3";
    const auto region = m_Atlas.Find(id, goal->map);
    if (!region)
    {
        return std::nullopt;
    }
    if (const auto portal = PortalGoal(*region))
    {
        return portal;
    }
    if (goal->position)
    {
        return WorldMarker{m_Atlas.ToWorld(*region, *goal->position),
                           goal->label.empty() ? "현재 목표" : goal->label,
                           goal->hint};
    }
    for (const auto& site : m_Atlas.Regions()[*region].local.landmarks)
    {
        if (site.object == goal->object)
        {
            return WorldMarker{
                m_Atlas.ToWorld(
                    *region, {site.position.x + goal->offset.x, site.position.y + goal->offset.y}),
                goal->label.empty() ? site.name : goal->label,
                goal->hint.empty() ? "[Space] 확인" : goal->hint};
        }
    }
    return std::nullopt;
}

bool GameSession::InCombat(WorldPosition world) const
{
    for (std::size_t i = 0; i < (m_OpenWorld ? m_Regions.size() : 1); ++i)
    {
        const auto local = m_OpenWorld ? m_Atlas.ToLocal(i, world) : world;
        const auto& enemies = !m_OpenWorld || i == m_Region ? m_Enemies : m_Regions[i].enemies;
        for (const auto& enemy : enemies.Enemies())
        {
            const float dx = enemy.position.x - local.x;
            const float dy = enemy.position.y - local.y;
            if (enemy.health > 0 && dx * dx + dy * dy < 600.f * 600.f)
            {
                return true;
            }
        }
    }
    return m_Chapter->ControlLocked();
}

void GameSession::AddWorldActors(SceneGraph& scene, WorldPosition player) const
{
    for (std::size_t region = 0; region < m_Regions.size(); ++region)
    {
        const auto& definition = m_Atlas.Regions()[region];
        const auto& enemies = region == m_Region ? m_Enemies : m_Regions[region].enemies;
        const auto& loot = region == m_Region ? m_Loot : m_Regions[region].loot;
        const auto* chapter = FindChapter(definition.chapter);
        scene.SetDynamicOrigin(definition.origin);
        for (std::size_t i = 0; i < enemies.Enemies().size(); ++i)
        {
            const auto& enemy = enemies.Enemies()[i];
            if (chapter->EnemyVisible(enemy))
            {
                scene.AddDynamicActor({enemy.position.x,
                                       enemy.position.y,
                                       6,
                                       1.f,
                                       static_cast<int>(i),
                                       static_cast<int>(region)});
            }
        }
        for (std::size_t i = 0; i < loot.Drops().size(); ++i)
        {
            const auto& drop = loot.Drops()[i];
            scene.AddDynamicActor({drop.position.x,
                                   drop.position.y,
                                   7,
                                   1.f,
                                   static_cast<int>(i),
                                   static_cast<int>(region)});
        }
    }
    scene.SetDynamicOrigin(WorldOrigin());
    m_Chapter->AddActors(scene, LocalPlayer(player));
    scene.SetDynamicOrigin({});
}

void GameSession::DrawWorldEnemy(Renderer& renderer, int region, int enemy, Point p) const
{
    if (region < 0)
    {
        CurrentChapter().DrawEnemy(renderer, m_Enemies.Enemies().at(enemy), p);
        return;
    }
    const auto index = static_cast<std::size_t>(region);
    const auto& enemies = index == m_Region ? m_Enemies : m_Regions.at(index).enemies;
    FindChapter(m_Atlas.Regions().at(index).chapter)
        ->DrawEnemy(renderer, enemies.Enemies().at(enemy), p);
}

const LootDrop& GameSession::WorldDrop(int region, int drop) const
{
    const auto& loot = region < 0 || static_cast<std::size_t>(region) == m_Region
                           ? m_Loot
                           : m_Regions.at(region).loot;
    return loot.Drops().at(drop);
}

WorldPosition GameSession::Checkpoint() const
{
    return m_Checkpoint;
}

bool GameSession::ReturnToCamp(WorldPosition player, WorldPosition& destination)
{
    if (m_Chapter->ControlLocked() || (!IsDefeated() && InCombat(player)))
    {
        ShowNotice("주변 적에게서 벗어난 뒤 야영지로 복귀할 수 있습니다.");
        return false;
    }
    if (IsDefeated())
    {
        Revive();
    }
    destination = m_Checkpoint;
    FollowPlayer(destination);
    m_Progress.Restore();
    m_Push = {};
    m_Invulnerable = 3.f;
    ShowNotice("야영지에서 회복했습니다. 조사·레벨·장비와 완료한 임무는 유지됩니다.");
    return true;
}

bool GameSession::DrawWorldLandmark(Renderer& renderer,
                                    const MapLandmark& site,
                                    Point p,
                                    float time) const
{
    if (!m_OpenWorld || site.worldRegion < 0)
    {
        return CurrentChapter().DrawLandmark(renderer, site, p, time);
    }
    if (site.object == -1)
    {
        return false;
    }
    const auto& region = m_Atlas.Regions().at(static_cast<std::size_t>(site.worldRegion));
    auto local = site;
    local.position = m_Atlas.ToLocal(static_cast<std::size_t>(site.worldRegion), site.position);
    return FindChapter(region.chapter)->DrawLandmark(renderer, local, p, time);
}
