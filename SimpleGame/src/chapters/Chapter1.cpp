#include "stdafx.h"
#include "Chapter1.h"
#include <algorithm>
#include <cmath>

namespace
{
    bool Near(WorldPosition a, WorldPosition b, float radius)
    {
        const float x = a.x - b.x, y = a.y - b.y;
        return x * x + y * y <= radius * radius;
    }
}

void Chapter1::Reset()
{
    m_State = Chapter1State{};
    m_Started.reset();
    ClearTravel();
    m_AlarmTime = m_WaveTime = m_PhaseTime = m_ConvoyTime = 0;
    m_Message = "호위 의뢰의 보수를 받으러 배급소에 왔다. Space로 주변을 확인한다.";
}

const Chapter1State& Chapter1::State() const
{
    return m_State;
}

const std::string& Chapter1::Message() const
{
    return m_Message;
}

void Chapter1::Fire(int trigger, const std::string& message)
{
    if (!m_State.triggers.test(trigger))
    {
        m_State.triggers.set(trigger);
        m_Message = message;
    }
}

WorldPosition Chapter1::Position(const LevelMap& map, ObjectId object)
{
    for (const auto& site : map.Layout().Landmarks())
    {
        if (site.object == object)
        {
            return site.position;
        }
    }
    return {};
}

bool Chapter1::ControlLocked() const
{
    return m_State.phase == Chapter1Phase::Execution || m_State.phase == Chapter1Phase::HeroPayoff;
}

void Chapter1::Update(float dt,
                      WorldPosition player,
                      bool interactionHeld,
                      const LevelMap& map,
                      EnemySystem& enemies,
                      PlayerProgression& progress)
{
    if (ControlLocked())
    {
        Payoff(dt, enemies, progress);
        return;
    }
    switch (m_State.map)
    {
    case Chapter1Maps::Village:
        Village(dt, player, map, enemies);
        break;
    case Chapter1Maps::Forest:
        Forest(dt, player, map, enemies);
        break;
    case Chapter1Maps::Mine:
        Mine(dt, player, interactionHeld, map, enemies, progress);
        break;
    case Chapter1Maps::Chapter2Route:
        break;
    }
}

void Chapter1::Village(float dt, WorldPosition, const LevelMap& map, EnemySystem& enemies)
{
    if (m_State.rationInteractionComplete && !m_State.villageAlert)
    {
        m_AlarmTime += dt;
        if (m_AlarmTime >= 4.f)
        {
            m_State.villageAlert = true;
            Fire(3, "동쪽·서쪽·북쪽에서 경보! 가장 가까운 농가를 지켜야 한다.");
        }
    }
    if (m_State.villageAlert && !m_State.farmRaidResolved)
    {
        auto center = Position(map, Chapter1Objects::FoodEvidence);
        center.y += 110;
        StartEncounter(Chapter1Encounter::FarmRaid, center, 3, enemies, map);
        if (EncounterComplete(Chapter1Encounter::FarmRaid, enemies))
        {
            m_State.farmRaidResolved = true;
            Fire(4, "습격은 끝났다. 물자와 주민이 사라졌다. 피해 농가 세 곳을 조사하자.");
        }
    }
}

void Chapter1::Forest(float dt, WorldPosition player, const LevelMap& map, EnemySystem& enemies)
{
    Fire(10, "선명한 자국과 깊은 자국이 갈린다. 비에 남은 물리적 흔적을 비교하자.");
    if (Near(player, Position(map, Chapter1Objects::LightTracks), 100.f))
    {
        m_State.wrongPathAmbushTriggered = true;
        Fire(12, "짐이 가벼운 미끼 수레였다! 후위 매복을 처리하고 되돌아갈 수 있다.");
        StartEncounter(Chapter1Encounter::WrongPathAmbush,
                       Position(map, Chapter1Objects::LightTracks),
                       2,
                       enemies,
                       map);
    }
    if (m_State.realTrackFound && Near(player, Position(map, Chapter1Objects::Ridge), 160.f))
    {
        m_State.convoySpotted = true;
        Fire(13, "다리 위 수송대에 포로가 있다. 수레를 태우지 말고 바퀴 고정핀을 빼자.");
        StartEncounter(
            Chapter1Encounter::Convoy, Position(map, Chapter1Objects::Wagon), 4, enemies, map);
    }
    if (m_State.convoySpotted && !m_State.convoyDisabled)
    {
        m_ConvoyTime += dt;
        m_State.suppliesPreserved = (std::max)(25, 100 - static_cast<int>(m_ConvoyTime / 3.f));
    }
}

void Chapter1::Mine(float dt,
                    WorldPosition player,
                    bool held,
                    const LevelMap& map,
                    EnemySystem& enemies,
                    PlayerProgression& progress)
{
    if (m_State.grumaDefeated)
    {
        return;
    }
    if (m_State.mineEntryRoute == Chapter1MineEntryRoute::None)
    {
        if (Near(player, Position(map, Chapter1Objects::FrontGate), 120.f))
        {
            m_State.mineEntryRoute = Chapter1MineEntryRoute::FrontGate;
            Fire(20, "정문 돌파. 경비 밀도가 높지만 길은 짧다.");
        }
        else if (Near(player, Position(map, Chapter1Objects::Drain), 120.f))
        {
            m_State.mineEntryRoute = Chapter1MineEntryRoute::Drainage;
            Fire(20, "배수로 잠입. 우회로를 통해 중계 창고로 향한다.");
        }
    }
    if (m_State.mineEntryRoute == Chapter1MineEntryRoute::FrontGate)
    {
        StartEncounter(Chapter1Encounter::MineGate,
                       Position(map, Chapter1Objects::FrontGate),
                       4,
                       enemies,
                       map);
    }
    if (Near(player, Position(map, Chapter1Objects::Training), 200.f) && !m_State.grumaDuelStarted)
    {
        StartEncounter(Chapter1Encounter::InternalPatrol,
                       Position(map, Chapter1Objects::Training),
                       3,
                       enemies,
                       map);
    }
    if (!m_State.grumaDuelStarted && Near(player, Arena(), 260.f))
    {
        // Gruma dismisses guards: the duel genuinely contains only the commander.
        enemies.Clear();
        m_State.grumaDuelStarted = true;
        m_State.phase = Chapter1Phase::Duel;
        StartEncounter(Chapter1Encounter::GrumaDuel, Arena(), 1, enemies, map);
        Fire(23, "그르마: 숨어 있던 게 아니다. 군단을 만들고 있었지. — 1:1 결투");
    }
    BossBattle(dt, player, held, map, enemies, progress);
}

void Chapter1::BossBattle(float dt,
                          WorldPosition player,
                          bool held,
                          const LevelMap& map,
                          EnemySystem& enemies,
                          PlayerProgression& progress)
{
    FieldEnemy* boss = enemies.Boss();
    if (!boss || m_State.grumaDefeated)
    {
        return;
    }
    if (boss->health <= 450 && !m_State.grumaArmorBroken)
    {
        m_State.grumaArmorBroken = boss->armorBroken = true;
        Fire(24, "그르마의 흉갑이 부서졌다. 상처가 드러난다.");
    }
    if (boss->health <= 300 && !m_State.grumaLegInjured)
    {
        m_State.grumaLegInjured = boss->legInjured = true;
        Fire(25, "그르마가 다리를 절기 시작한다. 개인전의 승기가 기울었다.");
    }
    if (m_State.grumaLegInjured && m_State.phase == Chapter1Phase::Duel)
    {
        m_State.phase = Chapter1Phase::ArmyCommand;
        Fire(26,
             "그르마: 나는 군단의 주인이다! 목표 변경: 치료병 처치 · 종 3개 파괴 · 증원문 차단");
        StartEncounter(Chapter1Encounter::ArmyCommand, Arena(), 3, enemies, map);
    }
    if (m_State.phase != Chapter1Phase::ArmyCommand)
    {
        return;
    }
    enemies.SetSupportEnabled(m_State.bellHealth[1] > 0);
    m_WaveTime += dt;
    if (!m_State.reinforcementGateClosed && m_State.commandBellsDestroyed < 3 &&
        m_WaveTime >= (m_State.bellHealth[0] <= 0 ? 12.f : 6.f))
    {
        m_WaveTime = 0;
        Reinforce(Position(map, Chapter1Objects::ReinforcementGate),
                  m_State.commandBellsDestroyed,
                  enemies,
                  map);
    }
    const bool healerAlive =
        std::any_of(enemies.Enemies().begin(),
                    enemies.Enemies().end(),
                    [](const FieldEnemy& enemy)
                    {
                        return enemy.role == EnemyRole::Healer && enemy.health > 0;
                    });
    if (held && m_State.commandBellsDestroyed == 3 && !healerAlive &&
        Near(player, Position(map, Chapter1Objects::ReinforcementGate), 105.f))
    {
        m_State.gateProgress = (std::min)(GateHoldSeconds, m_State.gateProgress + dt);
    }
    else
    {
        m_State.gateProgress = 0;
    }
    if (m_State.gateProgress >= GateHoldSeconds && progress.Health() > 0)
    {
        m_State.reinforcementGateClosed = true;
        m_State.strategicMissionSuccess = true;
        m_State.phase = Chapter1Phase::Execution;
        m_PhaseTime = 0;
        Fire(30, "전략 임무 성공! 증원문이 닫혔다. 군단 본대가 고립됐다.");
    }
}

void Chapter1::Payoff(float dt, EnemySystem& enemies, PlayerProgression& progress)
{
    if (!m_State.strategicMissionSuccess)
    {
        return;
    }
    m_PhaseTime += dt;
    if (m_State.phase == Chapter1Phase::Execution)
    {
        if (m_PhaseTime >= 2.f)
        {
            progress.Damage((std::max)(0, progress.Health() - 1));
            m_State.playerIncapacitated = true;
            Fire(31, "누적 부상으로 무릎이 꺾인다. 그르마가 마지막 처형 공격을 준비한다.");
        }
        if (m_PhaseTime >= 5.f && m_State.playerIncapacitated)
        {
            m_State.heroArrived = true;
            m_State.phase = Chapter1Phase::HeroPayoff;
            m_PhaseTime = 0;
            Fire(32, "여성 용사가 처형을 막는다. 구출 주민이 전한 표식과 폐광 위치를 따라왔다.");
        }
        return;
    }
    if (m_PhaseTime >= 8.f && m_PhaseTime < 16.f)
    {
        m_Message = "그르마가 종과 치료병을 찾지만 응답이 없다. 닫힌 문 밖 군단은 들어오지 못한다.";
    }
    else if (m_PhaseTime >= 16.f && m_PhaseTime < 26.f)
    {
        m_Message = "그르마가 돌진하지만 다리 상처로 자세가 무너진다. 용사가 빈틈으로 반격한다.";
    }
    else if (m_PhaseTime >= 26.f && m_PhaseTime < 35.f)
    {
        m_Message =
            "부서진 흉갑을 노린 마지막 일격. 플레이어가 만든 상처와 고립 조건이 승리를 완성한다.";
    }
    if (m_PhaseTime >= 35.f)
    {
        if (auto* boss = enemies.Boss())
        {
            boss->health = 0;
        }
        enemies.RemoveEncounter(static_cast<std::size_t>(Chapter1Encounter::ArmyCommand));
        m_State.grumaDefeated = true;
        m_State.phase = Chapter1Phase::Aftermath;
        progress.Heal(progress.MaxHealth() / 3);
        Fire(33, "그르마 사망. 용사가 응급 처치를 마쳤다. 지휘 장부를 함께 조사하자.");
    }
}

void Chapter1::StrikeObjects(WorldPosition player, bool faceLeft, int damage, const LevelMap& map)
{
    if (m_State.phase != Chapter1Phase::ArmyCommand)
    {
        return;
    }
    const ObjectId bells[] = {
        Chapter1Objects::BellA, Chapter1Objects::BellB, Chapter1Objects::BellC};
    for (int i = 0; i < 3; ++i)
    {
        const auto p = Position(map, bells[i]);
        const float dx = (p.x - player.x) * (faceLeft ? -1.f : 1.f);
        const float dy = p.y - player.y;
        if (m_State.bellHealth[i] > 0 && dx >= -18 && dx * dx + dy * dy * 2 <= 95.f * 95.f &&
            map.ClearPath(player, p))
        {
            m_State.bellHealth[i] = (std::max)(0, m_State.bellHealth[i] - damage);
            if (m_State.bellHealth[i] == 0)
            {
                ++m_State.commandBellsDestroyed;
                Fire(27 + i,
                     i == 0   ? "첫 종 파괴: 근접 증원 간격 증가."
                     : i == 1 ? "둘째 종 파괴: 고지대 지원과 치료 신호 중단."
                              : "셋째 종 파괴: 새 증원 호출 중단. 입구 쪽에서 전투 소리가 들린다.");
            }
        }
    }
}

std::string Chapter1::VillageInteraction(ObjectId object, PlayerInventory& inventory)
{
    if (object == Chapter1Objects::Ration && !m_State.rationInteractionComplete)
    {
        inventory.AddGold(40);
        m_State.rationInteractionComplete = true;
        Fire(1, "호위 보수 40G를 받았다. 원한다면 구호 상자에 10G를 기부할 수 있다.");
    }
    else if (object == Chapter1Objects::Donation)
    {
        if (!m_State.rationInteractionComplete || m_State.reliefDonationMade ||
            !inventory.SpendGold(10))
        {
            return "보수를 받은 뒤 한 번만 10G를 기부할 수 있다.";
        }
        m_State.reliefDonationMade = true;
        Fire(2, "구호 상자에 보수 일부를 넣었다. 배급 담당자가 농가 구호 물자를 마련한다.");
    }
    else if (object == Chapter1Objects::FoodEvidence || object == Chapter1Objects::RepairEvidence ||
             object == Chapter1Objects::TransportEvidence)
    {
        if (!m_State.farmRaidResolved)
        {
            return "먼저 농가를 습격한 고블린을 몰아내야 한다.";
        }
        const int index = object == Chapter1Objects::FoodEvidence     ? 0
                          : object == Chapter1Objects::RepairEvidence ? 1
                                                                      : 2;
        m_State.investigationEvidence.set(index);
        m_State.investigationEvidenceCount =
            static_cast<int>(m_State.investigationEvidence.count());
        if (m_State.investigationEvidenceCount == 3)
        {
            m_State.northTrailUnlocked = true;
            Fire(
                5,
                "같은 군단 표식. 식량·의료·정비·수송이 연결된다. 도둑질이 아니라 보급이야. 북쪽 숲길로 간다.");
        }
    }
    else if (object == Chapter1Objects::NorthExit)
    {
        if (!m_State.northTrailUnlocked)
        {
            return "농가 세 곳에서 무엇이 사라졌는지 조사해야 한다.";
        }
        Fire(6, "무거운 수레 자국을 따라 북쪽 숲길로 이동한다.");
        Travel(Chapter1Maps::Forest);
    }
    return {};
}

std::string Chapter1::ForestInteraction(ObjectId object, const EnemySystem& enemies)
{
    const ObjectId clues[] = {Chapter1Objects::LightTracks,
                              Chapter1Objects::HeavyTracks,
                              Chapter1Objects::Grain,
                              Chapter1Objects::Bark,
                              Chapter1Objects::Rope};
    for (int i = 0; i < 5; ++i)
    {
        if (object == clues[i])
        {
            m_State.trackingClues.set(i);
        }
    }
    if (m_State.trackingClues.test(1) && m_State.trackingClues.test(2) &&
        m_State.trackingClues.test(3) && m_State.trackingClues.test(4))
    {
        m_State.realTrackFound = true;
        Fire(11, "깊은 수레 자국·곡물·긁힌 나무·젖은 밧줄이 같은 경로를 가리킨다. 능선으로 가자.");
    }
    if (object == Chapter1Objects::Wagon)
    {
        if (!m_State.convoySpotted)
        {
            return "추적 단서를 확인하고 능선에서 수송대를 관찰해야 한다.";
        }
        m_State.convoyDisabled = true;
        Fire(14, "고정핀을 뽑았다. 수레가 멈췄고 군수품을 보존했다.");
    }
    else if (object == Chapter1Objects::Prisoner)
    {
        if (!m_State.convoyDisabled || !EncounterComplete(Chapter1Encounter::Convoy, enemies))
        {
            return "수레를 멈추고 호위병을 처리해야 안전하게 결박을 풀 수 있다.";
        }
        m_State.prisonersReleased = true;
        Fire(15, "포로 결박을 풀었다. 주민들이 서로 부축하며 마을로 돌아간다.");
    }
    else if (object == Chapter1Objects::Invoice)
    {
        if (!m_State.prisonersReleased)
        {
            return "수송대 호위병을 처리하고 주민을 구출해야 한다.";
        }
        m_State.mineEvidenceAcquired = true;
        Fire(
            16,
            "폐광 표시가 난 송장 확보. 주민에게 군단 표식과 폐광 위치를 경비대에 전해 달라고 부탁했다.");
    }
    else if (object == Chapter1Objects::MineExit)
    {
        if (!m_State.mineEvidenceAcquired)
        {
            return "주민을 구출하고 폐광 송장을 확보해야 한다.";
        }
        Fire(17, "송장에 기록된 폐광 기지로 이동한다.");
        Travel(Chapter1Maps::Mine);
    }
    return {};
}

std::string Chapter1::MineInteraction(ObjectId object, const LevelMap& map)
{
    const ObjectId evidence[] = {Chapter1Objects::Storage,
                                 Chapter1Objects::Training,
                                 Chapter1Objects::Repair,
                                 Chapter1Objects::ShiftBell,
                                 Chapter1Objects::TransportRecords,
                                 Chapter1Objects::CommandSeal};
    for (int i = 0; i < 6; ++i)
    {
        if (object == evidence[i])
        {
            m_State.mineInvestigation.set(i);
        }
    }
    if (object == Chapter1Objects::CommandSeal)
    {
        m_State.commandSealFound = true;
    }
    if (m_State.mineInvestigation.count() >= 3)
    {
        Fire(21,
             "병참·교대·광차 기록이 연결된다. 조사한 정보로 안전한 승강 경로를 이용할 수 있다.");
    }
    if (object == Chapter1Objects::Lift)
    {
        if (m_State.mineInvestigation.count() < 3)
        {
            return "조사 3개로 안전한 승강 경로를 찾을 수 있다. 조사 없이도 정면 갱도를 걸어 전진할 수 있다.";
        }
        auto destination = Arena();
        destination.y += 240.f;
        RequestTravel({{}, m_State.map, destination});
        Fire(22, "교대 주기에 맞춰 광차 승강로로 최심부에 접근한다.");
    }
    else if (object == Chapter1Objects::ReinforcementGate)
    {
        return "종 3개를 Ctrl 공격으로 파괴하고 치료병을 처치한 뒤, 장치 근처에서 Space를 4초 유지한다. 이동하며 유지할 수 있다.";
    }
    else if (object == Chapter1Objects::Ledger)
    {
        if (!m_State.grumaDefeated)
        {
            return "그르마와의 전투가 아직 끝나지 않았다.";
        }
        m_State.commandLedgerAcquired = true;
        Fire(
            34,
            "장부에 회색벽 요새 주변 오크 이동이 표시돼 있다. 용사: 다음에는 네가 혼자 군단과 싸우기 전에 도착하려고.");
    }
    else if (object == Chapter1Objects::Hero)
    {
        if (!m_State.grumaDefeated)
        {
            return "아직 동행할 인물이 없다.";
        }
        if (!m_State.commandLedgerAcquired)
        {
            return "용사: 주민들이 가져온 군단 표식 덕분에 왔어. 네가 약한 게 아니라, 혼자 군단과 싸운 거야. 장부를 같이 보자.";
        }
        m_State.aftermathDialogueComplete = true;
        return "플레이어: 명령은 안 받아. 용사: 나도 줄 생각 없어. — 동등한 협력자로 회색벽 요새를 향한다.";
    }
    else if (object == Chapter1Objects::ChapterExit)
    {
        if (!m_State.commandLedgerAcquired || !m_State.aftermathDialogueComplete)
        {
            return "장부를 조사하고 용사와 대화를 마쳐야 한다.";
        }
        m_State.chapter1Complete = true;
        m_State.phase = Chapter1Phase::Complete;
        Fire(35, "Chapter 1 종료. 회색벽 요새로 향하는 길에 들어섰다.");
        RequestTravel({"chapter2", std::nullopt, std::nullopt});
    }
    return {};
}

std::string Chapter1::Interact(const MapLandmark& site,
                               const LevelMap& map,
                               EnemySystem& enemies,
                               PlayerProgression&,
                               PlayerInventory& inventory)
{
    if (ControlLocked())
    {
        return {};
    }
    const auto before = m_Message;
    std::string result;
    switch (m_State.map)
    {
    case Chapter1Maps::Village:
        result = VillageInteraction(site.object, inventory);
        break;
    case Chapter1Maps::Forest:
        result = ForestInteraction(site.object, enemies);
        break;
    case Chapter1Maps::Mine:
        result = MineInteraction(site.object, map);
        break;
    case Chapter1Maps::Chapter2Route:
        break;
    }
    if (!result.empty())
    {
        return result;
    }
    return std::string(site.description) + (before != m_Message ? "\n" + m_Message : "");
}

const char* Chapter1::Title() const
{
    return "Chapter 1";
}

MapId Chapter1::InitialMap() const
{
    return Chapter1Maps::Village;
}

bool Chapter1::Complete() const
{
    return m_State.chapter1Complete;
}

void Chapter1::Travel(MapId next)
{
    RequestTravel({{}, next, std::nullopt});
}

void Chapter1::EnterMap(MapId map) noexcept
{
    m_State.map = map;
    m_Started.reset();
}

void Chapter1::Retry(EnemySystem& enemies)
{
    ClearTravel();
    enemies.Clear();
    m_Started.reset();
    m_State.gateProgress = 0;
    if (!m_State.grumaDefeated)
    {
        m_State.phase = Chapter1Phase::Exploration;
        m_State.grumaDuelStarted = m_State.grumaArmorBroken = m_State.grumaLegInjured = false;
        m_State.commandBellsDestroyed = 0;
        m_State.bellHealth = {60, 60, 60};
        for (int i = 23; i <= 33; ++i)
        {
            m_State.triggers.reset(i);
        }
    }
    m_Message = "현재 지역 입구로 복귀했다. 조사·소지품은 유지하고 미완료 전투를 다시 시작한다.";
}

std::string Chapter1::Objective() const
{
    if (m_State.chapter1Complete)
    {
        return "Chapter 1 완료 · 다음 목적지: 회색벽 요새 / 동등한 협력자로 동행";
    }
    if (ControlLocked())
    {
        return m_State.heroArrived ? "용사 payoff — 플레이어가 만든 승리 조건의 결과"
                                   : "전략 임무 성공 — 그르마의 마지막 공격";
    }
    if (m_State.map == Chapter1Maps::Village)
    {
        if (!m_State.rationInteractionComplete)
        {
            return "배급소에서 호위 보수 수령 [Space] · 구호 기부는 선택";
        }
        if (!m_State.farmRaidResolved)
        {
            return "가장 가까운 피해 농가에서 고블린 습격을 막기 [Ctrl]";
        }
        return "농가 증거 " + std::to_string(m_State.investigationEvidenceCount) +
               "/3 · 완료 후 북쪽 출구 [Space]";
    }
    if (m_State.map == Chapter1Maps::Forest)
    {
        if (!m_State.realTrackFound)
        {
            return "깊은 자국·곡물·나무껍질·밧줄 조사 [Space] · 미끼 길에서도 복귀 가능";
        }
        if (!m_State.convoyDisabled)
        {
            return "능선에서 관찰 → 다리 수송대 · 수레 고정핀 무력화 [Space]";
        }
        if (!m_State.prisonersReleased)
        {
            return "수송대 호위병 처치 → 포로 결박 해제 [Space]";
        }
        return "폐광 송장 확보 → 주민에게 보고 전달 → 폐광 길 [Space]";
    }
    if (m_State.phase == Chapter1Phase::Duel)
    {
        return "그르마 1:1 결투 · 갑옷과 다리에 피해를 입혀라";
    }
    if (m_State.phase == Chapter1Phase::ArmyCommand)
    {
        return "치료병 처치 · 종 " + std::to_string(m_State.commandBellsDestroyed) +
               "/3 [Ctrl] · 문 유지 " +
               std::to_string(static_cast<int>(m_State.gateProgress * 25)) + "% [Space]";
    }
    if (m_State.grumaDefeated)
    {
        return "장부 조사 → 용사와 대화 → 회색벽 요새로 출발 [Space]";
    }
    return "정문/배수로 선택 · 폐광 조사 " + std::to_string(m_State.mineInvestigation.count()) +
           "/6 (선택) · 승강로 또는 정면 갱도로 최심부";
}

bool Chapter1::EncounterStarted(Chapter1Encounter id) const
{
    return m_Started.test(static_cast<std::size_t>(id));
}

bool Chapter1::EncounterComplete(Chapter1Encounter id, const EnemySystem& enemies) const
{
    return EncounterStarted(id) && !enemies.Alive(static_cast<std::size_t>(id));
}

void Chapter1::SpawnGroup(Chapter1Encounter id,
                          WorldPosition center,
                          int count,
                          EnemyRole role,
                          EnemySystem& enemies,
                          const LevelMap& map)
{
    EnemyStats stats;
    if (role == EnemyRole::Healer)
    {
        stats.health = 65;
    }
    else if (role == EnemyRole::Boss)
    {
        stats.health = 600;
        stats.damage = 16;
        stats.speed = 95.f;
        stats.injuredSpeed = 48.f;
        stats.detectionRange = 900.f;
        stats.minimumHealth = 1; // Only Gruma's scripted payoff prevents ordinary lethal hits.
        stats.preserveCorpse = true;
    }
    for (int i = 0; i < count; ++i)
    {
        for (int attempt = 0; attempt < 100; ++attempt)
        {
            const int cell = attempt + i * 3;
            WorldPosition p{center.x + float(cell % 9 - 4) * 22,
                            center.y + float(cell / 9 - 2) * 22};
            if (!map.Blocked(p.x, p.y))
            {
                enemies.Add(p, static_cast<std::size_t>(id), role, stats);
                break;
            }
        }
    }
}

void Chapter1::StartEncounter(Chapter1Encounter id,
                              WorldPosition center,
                              int count,
                              EnemySystem& enemies,
                              const LevelMap& map)
{
    if (EncounterStarted(id))
    {
        return;
    }
    m_Started.set(static_cast<std::size_t>(id));
    SpawnGroup(id,
               center,
               count,
               id == Chapter1Encounter::GrumaDuel ? EnemyRole::Boss : EnemyRole::Melee,
               enemies,
               map);
    if (id == Chapter1Encounter::ArmyCommand)
    {
        SpawnGroup(id, {center.x + 80, center.y}, 1, EnemyRole::Healer, enemies, map);
        SpawnGroup(id, {center.x - 80, center.y}, 1, EnemyRole::Ranged, enemies, map);
    }
}

void Chapter1::Reinforce(WorldPosition center,
                         int bellsDestroyed,
                         EnemySystem& enemies,
                         const LevelMap& map)
{
    const auto living = std::count_if(enemies.Enemies().begin(),
                                      enemies.Enemies().end(),
                                      [](const FieldEnemy& enemy)
                                      {
                                          return enemy.health > 0;
                                      });
    if (bellsDestroyed >= 3 || living >= 10)
    {
        return;
    }
    SpawnGroup(Chapter1Encounter::ArmyCommand, center, 1, EnemyRole::Melee, enemies, map);
    if (bellsDestroyed < 2 && living < 9)
    {
        SpawnGroup(Chapter1Encounter::ArmyCommand, center, 1, EnemyRole::Ranged, enemies, map);
    }
}
