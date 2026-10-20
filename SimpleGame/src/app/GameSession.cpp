#include "stdafx.h"
#include "GameSession.h"
#include <algorithm>
#include "ChapterCatalog.h"
#include <stdexcept>
#include <utility>
#include <cmath>

GameSession::GameSession()
{
    ChapterCatalog::Register(*this);
}

void GameSession::RegisterChapter(std::string id, ChapterFactory factory)
{
    if (id.empty() || !factory || m_Chapters.count(id) != 0)
    {
        throw std::invalid_argument("챕터 등록에는 고유 ID와 생성 함수가 필요합니다.");
    }
    m_Chapters.emplace(std::move(id), ChapterSlot{std::move(factory), nullptr});
}

bool GameSession::TravelTo(const ChapterTravel& travel, LevelMap& map, WorldPosition& destination)
{
    if (m_OpenWorld)
    {
        ShowNotice("지역은 연결된 길을 따라 이동할 수 있습니다. M으로 목적지를 확인하세요.");
        return false;
    }
    const std::string id = travel.chapter.empty() ? m_ActiveChapterId : travel.chapter;
    const auto found = m_Chapters.find(id);
    if (found == m_Chapters.end())
    {
        ShowNotice("아직 연결되지 않은 챕터입니다.");
        return false;
    }

    // Construct and validate before replacing the current map or touching player data.
    auto& slot = found->second;
    std::unique_ptr<Chapter> created;
    try
    {
        if (!slot.instance)
        {
            created = slot.factory();
            if (!created)
            {
                throw std::runtime_error("챕터 생성 함수가 객체를 반환하지 않았습니다.");
            }
            created->Reset();
        }
        auto* next = slot.instance ? slot.instance.get() : created.get();
        const MapId mapId = travel.map.value_or(next->InitialMap());
        const bool changingMap = next != m_Chapter || map.ChapterId() != id || map.Map() != mapId;
        LevelMap candidate;
        if (changingMap)
        {
            auto definition = next->BuildMap(mapId);
            if (definition.id != mapId)
            {
                throw std::invalid_argument("요청한 지역과 지도 데이터의 ID가 다릅니다.");
            }
            candidate = map;
            candidate.Load(id, std::move(definition));
        }
        const auto& targetMap = changingMap ? candidate : map;
        const auto entry = travel.destination.value_or(targetMap.Layout().Definition().entry);
        if (!std::isfinite(entry.x) || !std::isfinite(entry.y) ||
            targetMap.Blocked(entry.x, entry.y))
        {
            throw std::invalid_argument("이동 위치가 막혀 있거나 올바르지 않습니다.");
        }

        // Allocate the next ID before committing. Map/string moves and EnterMap do not throw.
        std::string activeId = id;
        if (changingMap)
        {
            map = std::move(candidate);
            ClearMapActors();
            m_Notice.clear();
            m_NoticeTime = 0.f;
            m_Invulnerable = 0.f;
            next->EnterMap(mapId);
        }
        if (m_Chapter)
        {
            m_Chapter->ClearTravel();
        }
        next->ClearTravel();
        if (created)
        {
            slot.instance = std::move(created);
        }
        m_Chapter = next;
        m_ActiveChapterId = std::move(activeId);
        SynchronizeChapterHistory();
        destination = entry;
        return true;
    }
    catch (const std::exception& error)
    {
        ShowNotice(std::string("지역 이동을 완료하지 못했습니다: ") + error.what());
        return false;
    }
}

bool GameSession::ApplyPendingTravel(LevelMap& map, WorldPosition& destination)
{
    if (m_OpenWorld)
    {
        if (const auto pending = m_Chapter->PendingTravel())
        {
            const auto travel = *pending;
            m_Chapter->ClearTravel();
            return TravelThroughPortal(travel, destination);
        }
        m_Chapter->ClearTravel();
        return false;
    }
    if (!m_Chapter || !m_Chapter->PendingTravel())
    {
        return false;
    }
    const auto travel = *m_Chapter->PendingTravel();
    // A rejected request is reported once; the current chapter remains usable.
    m_Chapter->ClearTravel();
    return TravelTo(travel, map, destination);
}

const std::string& GameSession::ActiveChapterId() const
{
    return m_ActiveChapterId;
}

const Chapter* GameSession::FindChapter(const std::string& id) const
{
    const auto found = m_Chapters.find(id);
    return found == m_Chapters.end() ? nullptr : found->second.instance.get();
}

void GameSession::Reset(int profession, unsigned seed, LevelMap& map)
{
    m_OpenWorld = false;
    m_StartAtChapter2 = false;
    m_StartAtChapter3 = false;
    m_Regions.clear();
    m_Atlas.Clear();
    m_Profession = profession;
    m_Kills = 0;
    m_Invulnerable = 0.f;
    m_LandmarkInteractionTime = 0.f;
    m_LandmarkInteractionText.clear();
    m_Progress.Reset(profession);
    m_Inventory.Reset();
    m_Enemies.Reset(seed, map);
    m_Chapter = nullptr;
    for (auto& entry : m_Chapters)
    {
        entry.second.instance.reset();
    }
    m_ActiveChapterId.clear();
    WorldPosition destination;
    if (!TravelTo({ChapterCatalog::InitialChapter, std::nullopt, std::nullopt}, map, destination))
    {
        throw std::runtime_error("시작 챕터를 불러오지 못했습니다.");
    }
    m_Loot.Reset(seed);
    ShowNotice(std::string(m_Chapter->Title()) + " — " + m_Chapter->Objective());
}

void GameSession::Strike(int step, WorldPosition player, bool faceLeft, const LevelMap& sourceMap)
{
    const auto& map = m_OpenWorld ? m_Regions[m_Region].map : sourceMap;
    const auto worldPlayer = player;
    player = LocalPlayer(player);
    const float rawDamage =
        m_Progress.EffectiveAttack(m_Inventory.AttackBonus()) * (100 + (step - 1) * 25) / 100.f;
    const int damage = m_Progress.KnightAuraActive() ? static_cast<int>(std::lround(rawDamage))
                                                     : static_cast<int>(rawDamage);
    const float rangeScale = m_Progress.AttackRangeScale();
    if (m_Chapter->ControlLocked() || IsDefeated())
    {
        return;
    }
    m_Chapter->StrikeObjects(player, faceLeft, damage, map, rangeScale);
    RewardDefeated(m_Enemies.Strike(player, faceLeft, damage, map, rangeScale));
    if (m_OpenWorld)
    {
        for (std::size_t i = 0; i < m_Regions.size(); ++i)
        {
            if (i != m_Region)
            {
                auto& region = m_Regions[i];
                RewardDefeated(
                    region.enemies.Strike(
                        m_Atlas.ToLocal(i, worldPlayer), faceLeft, damage, region.map, rangeScale),
                    region.loot);
            }
        }
    }
}

void GameSession::StrikeProjectile(WorldPosition from,
                                   WorldPosition to,
                                   float radiusX,
                                   float radiusY,
                                   int damage,
                                   std::unordered_set<std::uint64_t>& hitTargets,
                                   const LevelMap& sourceMap,
                                   float height,
                                   std::optional<WorldPosition> sightOrigin)
{
    const auto& map = m_OpenWorld ? m_Regions[m_Region].map : sourceMap;
    const auto worldFrom = from, worldTo = to;
    from = LocalPlayer(from);
    to = LocalPlayer(to);
    if (m_Chapter->ControlLocked() || IsDefeated())
    {
        return;
    }
    RewardDefeated(m_Enemies.StrikeProjectile(
        from,
        to,
        radiusX,
        radiusY,
        damage,
        hitTargets,
        map,
        height,
        sightOrigin ? std::optional<WorldPosition>(LocalPlayer(*sightOrigin)) : std::nullopt));
    m_Chapter->StrikeProjectileObjects(
        from,
        to,
        radiusX,
        radiusY,
        damage,
        map,
        height,
        &hitTargets,
        sightOrigin ? std::optional<WorldPosition>(LocalPlayer(*sightOrigin)) : std::nullopt);
    if (m_OpenWorld)
    {
        for (std::size_t i = 0; i < m_Regions.size(); ++i)
        {
            if (i != m_Region)
            {
                auto& region = m_Regions[i];
                RewardDefeated(
                    region.enemies.StrikeProjectile(
                        m_Atlas.ToLocal(i, worldFrom),
                        m_Atlas.ToLocal(i, worldTo),
                        radiusX,
                        radiusY,
                        damage,
                        hitTargets,
                        region.map,
                        height,
                        sightOrigin ? std::optional<WorldPosition>(m_Atlas.ToLocal(i, *sightOrigin))
                                    : std::nullopt),
                    region.loot);
            }
        }
    }
}

void GameSession::RewardDefeated(const std::vector<WorldPosition>& defeated)
{
    RewardDefeated(defeated, m_Loot);
}

void GameSession::RewardDefeated(const std::vector<WorldPosition>& defeated, LootSystem& loot)
{
    for (const auto& position : defeated)
    {
        ++m_Kills;
        const int levels = m_Progress.AddExperience(LevelTuning::KillExperience);
        loot.Drop(position, m_Kills, m_Progress.Level());
        if (levels > 0)
        {
            ShowNotice("레벨 상승! 레벨 " + std::to_string(m_Progress.Level()) +
                       " · 체력·마나·주 능력치가 올랐습니다.");
        }
        else
        {
            ShowNotice("적 처치: 경험치 +25. 가까이 다가가 전리품을 끌어오세요.");
        }
    }
}

void GameSession::Update(float dt,
                         WorldPosition player,
                         const LevelMap& sourceMap,
                         bool interactionHeld)
{
    if (IsDefeated())
    {
        return;
    }
    const auto& map = m_OpenWorld ? m_Regions[m_Region].map : sourceMap;
    const auto worldPlayer = player;
    player = LocalPlayer(player);
    m_NoticeTime = (std::max)(0.f, m_NoticeTime - dt);
    if (m_NoticeTime == 0.f)
    {
        m_Notice.clear();
    }
    m_LandmarkInteractionTime = (std::max)(0.f, m_LandmarkInteractionTime - dt);
    if (m_LandmarkInteractionTime == 0.f)
    {
        m_LandmarkInteractionText.clear();
    }
    m_Invulnerable = (std::max)(0.f, m_Invulnerable - dt);
    const bool storyReady = StoryReady();
    if (storyReady)
    {
        m_Chapter->Update(dt, player, interactionHeld, map, m_Enemies, m_Progress);
    }
    RewardDefeated(m_Enemies.ConsumeScriptDefeats());
    const auto impact = m_Chapter->ConsumeImpact();
    int damage = m_Chapter->ControlLocked()
                     ? 0
                     : (std::max)(impact.damage,
                                  m_Enemies.Update(dt, player, map, m_OpenWorld ? 1500.f : 0.f));
    if (!m_Chapter->ControlLocked())
    {
        if (m_OpenWorld)
        {
            m_Regions[m_Region].population.Update(dt, player, m_Enemies);
            for (std::size_t i = 0; i < m_Regions.size(); ++i)
            {
                if (i != m_Region)
                {
                    auto& region = m_Regions[i];
                    const auto local = m_Atlas.ToLocal(i, worldPlayer);
                    region.population.Update(dt, local, region.enemies);
                    // Adjacent ordinary enemies remain live across region boundaries.
                    damage =
                        (std::max)(damage, region.enemies.Update(dt, local, region.map, 1500.f));
                }
            }
        }
        m_Push = impact.push;
    }
    if (damage > 0 && m_Invulnerable <= 0.f)
    {
        m_Progress.Damage(damage);
        m_Invulnerable = .8f;
        ShowNotice("피격! 적이 주황색일 때 물러나세요. 빨간 전리품은 체력을 회복합니다.");
    }
    // Defeat is resolved before pickups so a lethal hit cannot be silently undone.
    if (IsDefeated())
    {
        return;
    }
    if (m_Chapter->ControlLocked())
    {
        return;
    }
    CollectLoot(m_Loot.Update(dt, player, m_OpenWorld ? 1500.f : 0.f));
    if (m_OpenWorld)
    {
        for (std::size_t i = 0; i < m_Regions.size(); ++i)
        {
            if (i != m_Region)
            {
                CollectLoot(m_Regions[i].loot.Update(dt, m_Atlas.ToLocal(i, worldPlayer), 1500.f));
            }
        }
    }
}

void GameSession::CollectLoot(const std::vector<LootDrop>& drops)
{
    for (const auto& drop : drops)
    {
        switch (drop.kind)
        {
        case LootKind::Gold:
            m_Inventory.AddGold(drop.value);
            break;
        case LootKind::Equipment:
            m_Inventory.AddEquipment(m_Profession, drop.value);
            ShowNotice("장비 획득! 가장 강한 장비를 착용했습니다. I로 확인하세요.");
            break;
        case LootKind::Health:
            m_Progress.Heal(drop.value);
            break;
        }
    }
}

void GameSession::Revive()
{
    m_Chapter->Retry(m_Enemies);
    m_Push = {};
    m_Loot.Reset(1);
    m_Progress.Restore();
    m_Invulnerable = 3.f;
    ShowNotice("현재 지역 입구로 돌아왔습니다. 레벨·돈·장비는 유지됩니다.");
}

bool GameSession::IsDefeated() const
{
    return m_Progress.Health() <= 0 && !m_Chapter->ControlLocked();
}

bool GameSession::Complete() const
{
    return m_OpenWorld ? FindChapter("chapter3")->Complete() : m_Chapter->Complete();
}

std::string GameSession::Objective() const
{
    return StoryChapter().Objective();
}

void GameSession::Inspect(const MapLandmark& site, const LevelMap& sourceMap)
{
    if (IsDefeated() || m_Chapter->ControlLocked())
    {
        return;
    }
    auto local = site;
    if (m_OpenWorld)
    {
        if (site.worldRegion < 0)
        {
            return;
        }
        ActivateRegion(static_cast<std::size_t>(site.worldRegion));
        local.position = LocalPlayer(site.position);
        if (site.object == -1)
        {
            if (InCombat(site.position))
            {
                ShowNotice("야영지 주변의 적을 처리한 뒤 휴식하세요.");
                return;
            }
            m_Checkpoint = site.position;
            m_Progress.Restore();
            ShowNotice("휴식 완료 · 체력/마나 회복 · 이 야영지를 복귀 지점으로 등록했습니다.");
            return;
        }
        const auto& portals = m_Regions[m_Region].map.Layout().Definition().portals;
        const bool portal = std::any_of(portals.begin(),
                                        portals.end(),
                                        [&local](const MapPortal& item)
                                        {
                                            return item.object == local.object;
                                        });
        if (!StoryReady() && !portal)
        {
            ShowNotice(
                m_ActiveChapterId == "chapter3"
                    ? "챕터 3 조사는 협곡 구호지의 피해 보고와 벽화 탁본 검토 후 시작됩니다."
                    : "이 지역은 자유롭게 탐험할 수 있습니다. 주요 작전은 폐광의 장부와 용사 대화 후 시작됩니다.");
            return;
        }
    }
    const auto& map = m_OpenWorld ? m_Regions[m_Region].map : sourceMap;
    m_LandmarkInteractionText = m_Chapter->Interact(local, map, m_Enemies, m_Progress, m_Inventory);
    m_LandmarkInteractionPosition = site.position;
    m_LandmarkInteractionKind = site.kind;
    m_LandmarkInteractionTime = 9.f;
}

void GameSession::ShowSkillNotice(const std::string& text)
{
    ShowNotice(text);
}

void GameSession::ShowNotice(const std::string& text)
{
    m_Notice = text;
    m_NoticeTime = 4.f;
}

const std::string& GameSession::Notice() const
{
    return m_Notice.empty() ? m_Chapter->Message() : m_Notice;
}

bool GameSession::HasLandmarkInteraction() const
{
    return m_LandmarkInteractionTime > 0.f && !m_LandmarkInteractionText.empty();
}

const std::string& GameSession::LandmarkInteractionText() const
{
    return m_LandmarkInteractionText;
}

WorldPosition GameSession::LandmarkInteractionPosition() const
{
    return m_LandmarkInteractionPosition;
}

LandmarkKind GameSession::LandmarkInteractionKind() const
{
    return m_LandmarkInteractionKind;
}

const PlayerProgression& GameSession::Progress() const
{
    return m_Progress;
}

PlayerProgression& GameSession::Progress()
{
    return m_Progress;
}

const PlayerInventory& GameSession::Inventory() const
{
    return m_Inventory;
}

const EnemySystem& GameSession::Enemies() const
{
    return m_Enemies;
}

const LootSystem& GameSession::Loot() const
{
    return m_Loot;
}

int GameSession::Kills() const
{
    return m_Kills;
}

Chapter& GameSession::CurrentChapter()
{
    if (!m_Chapter)
    {
        throw std::logic_error("Start a chapter before accessing the session");
    }
    return *m_Chapter;
}

const Chapter& GameSession::CurrentChapter() const
{
    if (!m_Chapter)
    {
        throw std::logic_error("Start a chapter before accessing the session");
    }
    return *m_Chapter;
}

void GameSession::ClearMapActors()
{
    m_Push = {};
    m_Enemies.Clear();
    m_Loot.Reset(1);
    m_LandmarkInteractionText.clear();
    m_LandmarkInteractionTime = 0;
}

WorldPosition GameSession::ConsumePush()
{
    const auto result = m_Push;
    m_Push = {};
    return result;
}
