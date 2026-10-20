#include "stdafx.h"
#include "Chapter2Verification.h"
#include "Chapter2.h"
#include "ChapterCatalog.h"
#include "GameSession.h"
#include <algorithm>
#include <array>
#include <cmath>
#include <iostream>
#include <queue>
#include <set>
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

    void OpenChoices(Chapter2& chapter)
    {
        Require(!chapter.RecordEvent(Chapter2Event::JointFinisherEnded),
                "마무리 선행 조건이 우회됨");
        Require(chapter.RecordEvent(Chapter2Event::BriefingHeard), "브리핑 실패");
        Require(!chapter.RecordEvent(Chapter2Event::BriefingHeard), "브리핑 중복 처리");
        Require(chapter.RecordEvent(Chapter2Event::ScheduleRecovered), "일정표 조사 실패");
        Require(chapter.RecordEvent(Chapter2Event::SiegeMismatchFound), "공성 흔적 조사 실패");
        Require(!chapter.RecordEvent(Chapter2Event::PursuitAbandoned), "봉화 없이 추격 중지됨");
        Require(chapter.RecordEvent(Chapter2Event::FrontlineSupported), "전면전 결과 실패");
        Require(chapter.RecordEvent(Chapter2Event::BeaconDirectionFound), "봉화 조사 실패");
        Require(chapter.RecordEvent(Chapter2Event::PursuitAbandoned), "추격 중지 실패");
        Require(!chapter.RecordEvent(Chapter2Event::ConvoyPlanRead), "다른 맵에서 정보 조합됨");
        chapter.EnterMap(Chapter2Maps::Canyon);
        Require(chapter.RecordEvent(Chapter2Event::ConvoyPlanRead), "세 목표 해금 실패");
    }

    void VerifyOrders()
    {
        std::array<Chapter2Target, 3> order{
            Chapter2Target::Beacon, Chapter2Target::Workshop, Chapter2Target::Camp};
        int permutations = 0;
        do
        {
            Chapter2 chapter;
            chapter.Reset();
            OpenChoices(chapter);
            Require(!chapter.ChooseTarget(Chapter2Target::Count), "잘못된 목표 허용");
            Require(!chapter.RecordEvent(Chapter2Event::CommandOrdersRecovered),
                    "목표를 건너뛰고 명령서 획득");
            for (int rank = 0; rank < 3; ++rank)
            {
                const auto target = order[static_cast<std::size_t>(rank)];
                Require(chapter.ChooseTarget(target), "우선순위 선택 실패");
                Require(!chapter.ChooseTarget(target), "진행 중 목표 중복 선택");
                Require(chapter.StageLength(target) == static_cast<Chapter2StageLength>(rank),
                        "스테이지 밀도 순서 실패");
                Require(!chapter.ResolveTarget(order[static_cast<std::size_t>((rank + 1) % 3)]),
                        "다른 목표 결과 허용");
                EnemySystem enemies;
                chapter.Retry(enemies);
                Require(chapter.State().activeTarget == target, "재시도 시 선택 상태 소실");
                LevelMap targetMap;
                targetMap.Generate(743);
                targetMap.Load("chapter2", chapter.BuildMap(Chapter2Maps::Canyon));
                PlayerProgression progress;
                progress.Reset(0);
                for (int frame = 0; frame < 1200 && chapter.State().activeTarget; ++frame)
                {
                    const auto position =
                        Chapter2::DevicePosition(target, chapter.Combat().objectiveStep);
                    chapter.Update(.05f, position, true, targetMap, enemies, progress);
                    for (const auto& enemy : enemies.Enemies())
                    {
                        if (enemy.health > 0)
                        {
                            enemies.DamageEnemy(enemy.id, enemy.health);
                        }
                    }
                }
                Require(!chapter.State().activeTarget, "목표 전투·장치 단계가 완료되지 않음");
                Require(!chapter.ResolveTarget(target), "목표 결과 중복 반영");
                for (int remaining = rank + 1; remaining < 3; ++remaining)
                {
                    const auto index =
                        static_cast<std::size_t>(order[static_cast<std::size_t>(remaining)]);
                    Require(static_cast<int>(chapter.State().conditions[index]) ==
                                (std::min)(2, rank + 1),
                            "미선택 목표 악화 실패");
                }
            }
            Require(chapter.RecordEvent(Chapter2Event::CommandOrdersRecovered), "진입로 해금 실패");
            Require(chapter.RouteAvailable(Chapter2Route::Aqueduct), "진입 가능한 경로가 없음");
            Require(chapter.RouteAvailable(Chapter2Route::Chapel) ==
                        (order.front() == Chapter2Target::Camp),
                    "예배당 정보 조건 실패");
            chapter.EnterMap(Chapter2Maps::OldFortress);
            Require(!chapter.RecordEvent(Chapter2Event::MuralCopied), "진입로 선택 없이 벽화 획득");
            Require(chapter.SelectRoute(Chapter2Route::Aqueduct), "급수로 선택 실패");
            Require(chapter.RecordEvent(Chapter2Event::MuralCopied), "벽화 획득 실패");
            const bool bypassFirst = permutations % 2 == 0;
            Require(chapter.RecordEvent(bypassFirst ? Chapter2Event::BypassOpened
                                                    : Chapter2Event::DemolitionCut),
                    "첫 우회 장치 처리 실패");
            Require(!chapter.RecordEvent(Chapter2Event::HeroRejoined), "두 장치 처리 전 합류");
            Require(chapter.RecordEvent(bypassFirst ? Chapter2Event::DemolitionCut
                                                    : Chapter2Event::BypassOpened),
                    "둘째 우회 장치 처리 실패");
            LevelMap reunionMap;
            reunionMap.Generate(743u + permutations);
            reunionMap.Load("chapter2", chapter.BuildMap(Chapter2Maps::OldFortress));
            EnemySystem reunionEnemies;
            PlayerProgression reunionProgress;
            reunionProgress.Reset(0);
            PlayerInventory reunionInventory;
            const WorldPosition signal{0.f, -2470.f};
            Require(!reunionMap.Blocked(signal.x, signal.y), "Reunion signal is inside a wall");
            EnemyPathfinder pathfinder;
            std::vector<WorldPosition> route;
            Require(pathfinder.FindPath({-500.f, -2100.f}, signal, reunionMap, route),
                    "Hero cannot find an A* route to the reunion signal");
            chapter.Update(.05f, signal, false, reunionMap, reunionEnemies, reunionProgress);
            const auto& sites = reunionMap.Layout().Landmarks();
            const auto signalSite =
                std::find_if(sites.begin(),
                             sites.end(),
                             [](const MapLandmark& site)
                             {
                                 return site.object == Chapter2Objects::ReunionSignal;
                             });
            Require(signalSite != sites.end(), "Reunion signal is missing");
            chapter.Interact(
                *signalSite, reunionMap, reunionEnemies, reunionProgress, reunionInventory);
            for (int frame = 0; frame < 600 && chapter.State().phase == Chapter2Phase::Reunion;
                 ++frame)
            {
                const auto previous = chapter.Combat().hero;
                chapter.Update(.05f, signal, false, reunionMap, reunionEnemies, reunionProgress);
                Require(reunionMap.ClearPath(previous, chapter.Combat().hero),
                        "Hero crossed a wall while rejoining");
            }
            Require(chapter.State().phase == Chapter2Phase::Boss,
                    "Hero never physically reached the reunion signal");
            Require(!chapter.RecordEvent(Chapter2Event::ArmorBroken), "퇴로 차단 전 자세 붕괴");
            Require(!chapter.RecordEvent(Chapter2Event::CommandTargetDestroyed),
                    "자세 붕괴 전 최종 입력");
            Require(chapter.RecordEvent(Chapter2Event::RetreatBlocked), "퇴로 차단 실패");
            Require(chapter.RecordEvent(Chapter2Event::ArmorBroken), "자세 붕괴 실패");
            Require(!chapter.RecordEvent(Chapter2Event::JointFinisherEnded),
                    "플레이어 최종 입력 생략");
            Require(chapter.RecordEvent(Chapter2Event::CommandTargetDestroyed),
                    "플레이어 최종 입력 실패");
            Require(chapter.RecordEvent(Chapter2Event::JointFinisherEnded),
                    "공동 마무리 결과 실패");
            Require(chapter.State().outcome.bridgeDamage >= 1 &&
                        chapter.State().outcome.supplyLoss >= 1,
                    "필수 손실 누락");
            Require(chapter.State().outcome.refugeeMajoritySurvived, "피난민 대다수 생존 누락");
            Require(!chapter.Complete(), "사후 확인 전에 챕터 완료됨");
            chapter.EnterMap(Chapter2Maps::Canyon);
            Require(chapter.RecordEvent(Chapter2Event::AftermathObserved), "사후 피해 확인 실패");
            Require(chapter.RecordEvent(Chapter2Event::MuralReviewed), "후속 조사 목표 실패");
            const auto flags = chapter.ExportFlags();
            Require(chapter.Complete() && flags.at("C02_COMPLETE") == 1 &&
                        flags.at("C02_MURAL_EVIDENCE") == 1 &&
                        flags.at("C02_DEVELOPMENT_PREVIEW") == 0,
                    "후속 결과 플래그 누락");
            Require(flags.at("C02_BEACON_DELAY") == chapter.State().priority[0] &&
                        flags.at("C02_REFUGEE_LOSS") == chapter.State().priority[2],
                    "순서별 결과 불일치");
            chapter.Reset();
            Require(!chapter.Complete() && chapter.State().completedTargets == 0 &&
                        chapter.State().events.none() && !chapter.State().route &&
                        chapter.State().priority == std::array<int, 3>{-1, -1, -1},
                    "새 진행에 이전 결과가 남음");
            ++permutations;
        } while (std::next_permutation(order.begin(), order.end()));
        Require(permutations == 6, "6개 우선순위 조합을 검사하지 못함");
    }

    void VerifyMaps()
    {
        Chapter2 chapter;
        bool invalidMapRejected = false;
        try
        {
            chapter.BuildMap(999);
        }
        catch (const std::invalid_argument&)
        {
            invalidMapRejected = true;
        }
        Require(invalidMapRejected, "잘못된 지도 ID를 허용함");
        for (const auto id :
             {Chapter2Maps::Graywall, Chapter2Maps::Canyon, Chapter2Maps::OldFortress})
        {
            LevelMap map;
            map.Generate(743);
            map.Load("chapter2", chapter.BuildMap(id));
            Require(!map.Blocked(0.f, 0.f), "입구가 막힘");
            Require(map.Blocked(6000.f, 6000.f), "맵 경계가 열림");
            std::set<ObjectId> objects;
            for (const auto& site : map.Layout().Landmarks())
            {
                Require(objects.insert(site.object).second, "맵 내 오브젝트 ID 중복");
                Require(!map.Blocked(site.position.x, site.position.y), "상호작용 앵커가 막힘");
            }
            constexpr int step = 40;
            std::queue<std::pair<int, int>> pending;
            std::set<std::pair<int, int>> visited;
            pending.push({0, 0});
            visited.insert({0, 0});
            while (!pending.empty())
            {
                const auto cell = pending.front();
                pending.pop();
                for (const auto offset : {std::pair<int, int>{1, 0}, {-1, 0}, {0, 1}, {0, -1}})
                {
                    const auto next =
                        std::make_pair(cell.first + offset.first, cell.second + offset.second);
                    if (visited.count(next) || std::abs(next.first) > 50 || next.second < -100 ||
                        next.second > 35)
                    {
                        continue;
                    }
                    if (map.ClearPath({float(cell.first * step), float(cell.second * step)},
                                      {float(next.first * step), float(next.second * step)}))
                    {
                        visited.insert(next);
                        pending.push(next);
                    }
                }
            }
            for (const auto& site : map.Layout().Landmarks())
            {
                const bool reachable = std::any_of(
                    visited.begin(),
                    visited.end(),
                    [&](const auto& cell)
                    {
                        const WorldPosition p{float(cell.first * step), float(cell.second * step)};
                        const float dx = p.x - site.position.x, dy = p.y - site.position.y;
                        return dx * dx + dy * dy < 90.f * 90.f && map.ClearPath(p, site.position);
                    });
                Require(reachable, "상호작용 앵커에 도달할 수 없음");
            }
        }
    }

    void VerifyCombatCheckpoints()
    {
        Chapter2 chapter;
        chapter.Reset();
        LevelMap map;
        map.Generate(743);
        map.Load("chapter2", chapter.BuildMap(Chapter2Maps::Graywall));
        EnemySystem enemies;
        PlayerProgression progress;
        progress.Reset(0);
        chapter.RecordEvent(Chapter2Event::BriefingHeard);
        chapter.RecordEvent(Chapter2Event::SiegeMismatchFound);
        chapter.RecordEvent(Chapter2Event::ScheduleRecovered);
        chapter.RecordEvent(Chapter2Event::FrontlineSupported);
        chapter.Retry(enemies);
        chapter.Update(.05f, {730, -1800}, false, map, enemies, progress);
        Require(chapter.Combat().group == 203 && enemies.Alive(203),
                "전면전 이후 재시도 시 신호병이 복구되지 않음");

        enemies.Clear();
        EnemyStats stats;
        stats.health = 1;
        enemies.Add({0, 0}, 999, EnemyRole::Melee, stats);
        const auto id = enemies.Enemies().front().id;
        enemies.DamageEnemy(id, 10);
        enemies.DamageEnemy(id, 10);
        Require(enemies.ConsumeScriptDefeats().size() == 1, "용사 처치 보상 누락 또는 중복");
        Require(enemies.ConsumeScriptDefeats().empty(), "처치 보상 재지급");
    }

    void VerifySession()
    {
        GameSession session;
        LevelMap map;
        map.Generate(743);
        session.Reset(0, map.Seed(), map);
        WorldPosition destination;
        Require(session.TravelTo({"chapter2", std::nullopt, std::nullopt}, map, destination),
                "본편 C02 진입 실패");
        ChapterCatalog::RegisterDevelopment(session);
        session.Progress().DebugSetLevel(12);
        Require(session.TravelTo({"chapter2", std::nullopt, std::nullopt}, map, destination),
                "개발용 C02 진입 실패");
        Require(map.Map() == Chapter2Maps::Graywall && session.Progress().Level() == 12,
                "진입 시 맵/성장 상태 오류");
        auto& chapter = dynamic_cast<Chapter2&>(session.CurrentChapter());
        for (const auto event : {Chapter2Event::BriefingHeard,
                                 Chapter2Event::SiegeMismatchFound,
                                 Chapter2Event::ScheduleRecovered,
                                 Chapter2Event::FrontlineSupported,
                                 Chapter2Event::BeaconDirectionFound,
                                 Chapter2Event::PursuitAbandoned})
        {
            Require(chapter.RecordEvent(event), "이동 전 진행 조건 실패");
        }
        for (const auto& site : map.Layout().Landmarks())
        {
            if (site.object == Chapter2Objects::CanyonExit)
            {
                session.Inspect(site, map);
                break;
            }
        }
        Require(session.ApplyPendingTravel(map, destination) && map.Map() == Chapter2Maps::Canyon,
                "정상 협곡 연결로 이동 실패");
        Require(!session.ApplyPendingTravel(map, destination), "이동 중복 적용");
        Require(chapter.RecordEvent(Chapter2Event::ConvoyPlanRead), "작전 지도 확인 실패");
        const std::array<ObjectId, 3> choices{Chapter2Objects::ChooseBeacon,
                                              Chapter2Objects::ChooseWorkshop,
                                              Chapter2Objects::ChooseCamp};
        for (std::size_t index = 0; index < choices.size(); ++index)
        {
            const auto& landmarks = map.Layout().Landmarks();
            const auto site = std::find_if(landmarks.begin(),
                                           landmarks.end(),
                                           [object = choices[index]](const MapLandmark& landmark)
                                           {
                                               return landmark.object == object;
                                           });
            Require(site != landmarks.end(), "협곡 목표 선택지가 없음");
            session.Inspect(*site, map);
            const auto target = static_cast<Chapter2Target>(index);
            Require(chapter.State().activeTarget == target && chapter.PendingTravel(),
                    "협곡 목표 선택 후 이동 요청이 없음");
            Require(session.ApplyPendingTravel(map, destination) &&
                        map.Map() == Chapter2Maps::Canyon &&
                        !map.Blocked(destination.x, destination.y),
                    "협곡 목표 진입 지점으로 이동 실패");
            const auto center = Chapter2::DevicePosition(target, 0);
            Require(std::hypot(destination.x - center.x, destination.y - center.y) < 500.f,
                    "선택한 목표와 다른 위치로 이동함");
            Require(!session.ApplyPendingTravel(map, destination), "목표 이동 중복 적용");
            Require(chapter.ResolveTarget(target), "다음 목표 선택 준비 실패");
        }
        Require(session.CurrentChapter().ExportFlags().at("C02_COMPLETE") == 0,
                "지도만 이동해서 완료됨");
        Require(session.TravelTo({"chapter1", std::nullopt, std::nullopt}, map, destination),
                "C01 복귀 실패");
        Require(session.FindChapter("chapter2")->ExportFlags().at("C02_DEVELOPMENT_PREVIEW") == 0,
                "챕터 이동 후 결과 조회 실패");
    }
}

int RunChapter2Verification()
{
    try
    {
        VerifyOrders();
        VerifyMaps();
        VerifySession();
        VerifyCombatCheckpoints();
        std::cout
            << "PASS 챕터 2 기반: 6개 순서 / 선행 조건 / 결과 상태 / 3개 맵 / 공통 세션 연결\n";
        return 0;
    }
    catch (const std::exception& error)
    {
        std::cerr << "FAIL 챕터 2 기반: " << error.what() << "\n";
        return 1;
    }
}
