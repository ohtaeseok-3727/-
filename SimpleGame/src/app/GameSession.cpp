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

void GameSession::Strike(int step, WorldPosition player, bool faceLeft, const LevelMap& map)
{
    const int damage =
        (m_Progress.Attack() + m_Inventory.AttackBonus()) * (100 + (step - 1) * 25) / 100;
    if (m_Chapter->ControlLocked())
    {
        return;
    }
    m_Chapter->StrikeObjects(player, faceLeft, damage, map);
    RewardDefeated(m_Enemies.Strike(player, faceLeft, damage, map));
}

void GameSession::StrikeProjectile(WorldPosition from,
                                   WorldPosition to,
                                   float radiusX,
                                   float radiusY,
                                   int damage,
                                   std::unordered_set<std::uint64_t>& hitTargets,
                                   const LevelMap& map)
{
    if (m_Chapter->ControlLocked())
    {
        return;
    }
    RewardDefeated(m_Enemies.StrikeProjectile(from, to, radiusX, radiusY, damage, hitTargets, map));
    m_Chapter->StrikeProjectileObjects(from, to, radiusX, radiusY, damage, map);
}

void GameSession::RewardDefeated(const std::vector<WorldPosition>& defeated)
{
    for (const auto& position : defeated)
    {
        ++m_Kills;
        const int levels = m_Progress.AddExperience(LevelTuning::KillExperience);
        m_Loot.Drop(position, m_Kills, m_Progress.Level());
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

void GameSession::Update(float dt, WorldPosition player, const LevelMap& map, bool interactionHeld)
{
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
    m_Chapter->Update(dt, player, interactionHeld, map, m_Enemies, m_Progress);
    RewardDefeated(m_Enemies.ConsumeScriptDefeats());
    const auto impact = m_Chapter->ConsumeImpact();
    const int damage = m_Chapter->ControlLocked()
                           ? 0
                           : (std::max)(impact.damage, m_Enemies.Update(dt, player, map));
    if (!m_Chapter->ControlLocked())
    {
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
    for (const auto& drop : m_Loot.Update(dt, player))
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
    m_Progress.Heal(m_Progress.MaxHealth());
    m_Invulnerable = 3.f;
    ShowNotice("현재 지역 입구로 돌아왔습니다. 레벨·돈·장비는 유지됩니다.");
}

bool GameSession::IsDefeated() const
{
    return m_Progress.Health() <= 0 && !m_Chapter->ControlLocked();
}

bool GameSession::Complete() const
{
    return m_Chapter->Complete();
}

std::string GameSession::Objective() const
{
    return m_Chapter->Objective();
}

void GameSession::Inspect(const MapLandmark& site, const LevelMap& map)
{
    m_LandmarkInteractionText = m_Chapter->Interact(site, map, m_Enemies, m_Progress, m_Inventory);
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
