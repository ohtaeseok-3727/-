#include "stdafx.h"
#include "Chapter2.h"
#include <stdexcept>

namespace
{
    MapLandmark Site(ObjectId id,
                     WorldPosition position,
                     const char* name,
                     const char* description,
                     LandmarkKind kind = LandmarkKind::Sign)
    {
        return {kind, position, name, description, 0.f, id};
    }

}

MapDefinition Chapter2::BuildMap(MapId map) const
{
    switch (map)
    {
    case Chapter2Maps::Graywall:
        return GraywallMap();
    case Chapter2Maps::Canyon:
        return CanyonMap();
    case Chapter2Maps::OldFortress:
        return FortressMap();
    default:
        throw std::invalid_argument("챕터 2에 등록되지 않은 지역입니다.");
    }
}

MapDefinition Chapter2::GraywallMap()
{
    MapDefinition map;
    map.id = Chapter2Maps::Graywall;
    map.fieldEnemyHealth = LevelTuning::OrcHealth;
    map.title = "회색벽 요새와 전면 방어선";
    // Coordinates and corridor widths are provisional implementation values, not canon.
    map.areas = {{MapRegion::Village, "회색벽 진입", {0, 100}, 500, 340},
                 {MapRegion::Outpost, "ZONE-01 작전실", {0, -450}, 250, 210},
                 {MapRegion::Outpost, "ZONE-02 성벽 관측", {-500, -850}, 250, 190},
                 {MapRegion::Battlefield, "ZONE-03 무너진 감시초소", {550, -950}, 300, 250},
                 {MapRegion::Battlefield, "ZONE-04 전면 방어선", {0, -1550}, 600, 280},
                 {MapRegion::Outpost, "ZONE-05 신호 측면", {730, -1800}, 240, 220},
                 {MapRegion::Outpost, "협곡 연결로", {0, -2250}, 220, 190}};
    map.roads = {{{0, 0}, {0, -450}, 100},
                 {{0, -450}, {-500, -850}, 100},
                 {{-500, -850}, {550, -950}, 100},
                 {{0, -450}, {0, -1550}, 110},
                 {{550, -950}, {0, -1550}, 110},
                 {{0, -1550}, {730, -1800}, 105},
                 {{0, -1550}, {0, -2250}, 110}};
    map.landmarks = {
        Site(Chapter2Objects::Briefing,
             {0, -450},
             "작전 브리핑",
             "성문 정면 방어 가정을 확인하고 외곽 조사에 나섭니다.",
             LandmarkKind::Board),
        Site(Chapter2Objects::Tracks,
             {-500, -850},
             "발자국과 수레 흔적",
             "왕복 흔적에 비해 대형 공성 장비와 장기 보급 흔적이 부족합니다.",
             LandmarkKind::Tracks),
        Site(Chapter2Objects::Schedule,
             {550, -950},
             "정찰병 일정표",
             "왕의 협곡 통과 시각 일부를 회수합니다.",
             LandmarkKind::Records),
        Site(Chapter2Objects::Frontline,
             {-220, -1550},
             "전면 방어선",
             "용사가 정면을 밀어내는 동안 플레이어가 전열을 지원합니다."),
        Site(Chapter2Objects::Signal,
             {730, -1800},
             "협곡 봉화 방향",
             "후퇴 신호가 협곡으로 향합니다. 공성대가 없는 정면전과 일정표를 비교합니다.",
             LandmarkKind::Banner),
        Site(Chapter2Objects::StopPursuit,
             {220, -1550},
             "추격 중지",
             "용사는 전멸시킬 수 있는 적을 놓치고 플레이어와 협곡으로 향합니다."),
        Site(Chapter2Objects::CanyonExit,
             {0, -2250},
             "왕의 협곡",
             "일정표와 봉화를 근거로 추격을 중지한 뒤 이동합니다.")};
    map.encounters = {{400, -1000}, {-180, -1550}, {730, -1800}};
    return map;
}

MapDefinition Chapter2::CanyonMap()
{
    MapDefinition map;
    map.id = Chapter2Maps::Canyon;
    map.fieldEnemyHealth = LevelTuning::OrcHealth;
    map.title = "왕의 협곡 작전 구역";
    map.areas = {{MapRegion::Outpost, "ZONE-06 관측소·작전 지도", {0, 0}, 560, 400},
                 {MapRegion::Outpost, "ZONE-07 봉화탑", {0, -1250}, 330, 300},
                 {MapRegion::Battlefield, "ZONE-08 공성 작업장", {-1100, -350}, 340, 300},
                 {MapRegion::Outpost, "ZONE-09 포로 수용소", {1100, -350}, 340, 300},
                 {MapRegion::Outpost, "ZONE-10 교량·호송로", {650, 450}, 240, 160},
                 {MapRegion::Village, "ZONE-11 구호지", {0, 800}, 520, 260},
                 {MapRegion::Outpost, "옛 성채 연결로", {0, -2050}, 250, 190}};
    map.roads = {{{0, 0}, {0, -1250}, 100},
                 {{0, 0}, {-1100, -350}, 105},
                 {{0, 0}, {1100, -350}, 105},
                 {{-1100, -350}, {0, 800}, 100},
                 {{1100, -350}, {650, 450}, 100},
                 {{650, 450}, {0, 800}, 100},
                 {{0, 0}, {0, 800}, 110},
                 {{0, -1250}, {0, -2050}, 110}};
    map.water = {{{650, 450}, 260, 90}};
    map.landmarks = {
        Site(
            Chapter2Objects::TacticalMap,
            {0, -130},
            "장부·일정표·봉화 조합",
            "세 위험을 먼저 확인하세요. 봉화: 정예 증원 / 작업장: 교량·보급 손실 / 수용소: 피난민·부상자 피해. 권장 순서는 없습니다.",
            LandmarkKind::Board),
        Site(Chapter2Objects::ChooseBeacon,
             {-280, -160},
             "봉화탑 대응 선택",
             TargetRisk(Chapter2Target::Beacon)),
        Site(Chapter2Objects::ChooseWorkshop,
             {0, -350},
             "작업장 대응 선택",
             TargetRisk(Chapter2Target::Workshop)),
        Site(Chapter2Objects::ChooseCamp,
             {280, -160},
             "수용소 대응 선택",
             TargetRisk(Chapter2Target::Camp)),
        Site(Chapter2Objects::BeaconControl,
             {0, -1250},
             "봉화 신호 장치",
             "봉화탑 경계를 격파한 뒤 신호를 차단합니다.",
             LandmarkKind::Banner),
        Site(Chapter2Objects::WorkshopControl,
             {-1100, -350},
             "폭약·지지대",
             "공성 작업장 경계를 격파한 뒤 폭약을 해체하고 지지대를 보호합니다.",
             LandmarkKind::Workbench),
        Site(
            Chapter2Objects::CampControl,
            {1100, -350},
            "포로 수용소 문",
            "포로 수용소 경계를 격파한 뒤 피난민과 부상자를 구출합니다. 정확한 인원수는 미정입니다.",
            LandmarkKind::Palisade),
        Site(Chapter2Objects::CommandOrders,
             {250, 800},
             "바르칸 지휘소 명령서",
             "세 목표를 모두 처리하면 옛 성채 위치와 사용 가능한 진입로를 확인합니다.",
             LandmarkKind::Records),
        Site(Chapter2Objects::FortressExit,
             {0, -2050},
             "옛 인간 성채",
             "바르칸 지휘소 위치를 확인한 뒤 이동합니다."),
        Site(
            Chapter2Objects::ReliefReport,
            {-250, 800},
            "구호지와 손실 확인",
            "전령: 전선을 버렸다는 보고가 왔습니다. 생존자: 그 판단이 없었다면 우리는 여기 없었을 겁니다."),
        Site(
            Chapter2Objects::MuralReview,
            {0, 980},
            "벽화 탁본 검토",
            "일곱 자리와 널리 알려진 여섯 이름의 모순을 남깁니다. 지워진 인물의 정체는 단정하지 않습니다.",
            LandmarkKind::Records)};
    // Register secondary devices as anchors so procedural clutter cannot block them.
    for (int target = 0; target < 3; ++target)
    {
        for (int step = 1; step < 3; ++step)
        {
            map.landmarks.push_back(
                Site(100 + target * 3 + step,
                     DevicePosition(static_cast<Chapter2Target>(target), step),
                     target == 0   ? "중계 신호 장치"
                     : target == 1 ? "폭약·교량 지지대"
                                   : "대피 지점",
                     "현재 대응 위치 표식을 따라 장치를 처리하거나 대피를 보호하세요."));
        }
    }
    map.encounters = {{0, -1140}, {-980, -350}, {980, -350}};
    return map;
}

MapDefinition Chapter2::FortressMap()
{
    MapDefinition map;
    map.id = Chapter2Maps::OldFortress;
    map.fieldEnemyHealth = LevelTuning::OrcHealth;
    map.title = "옛 인간 성채";
    map.areas = {{MapRegion::Outpost, "ZONE-12 진입 분기", {0, 0}, 850, 400},
                 {MapRegion::GuardTunnel, "급수로", {-650, -700}, 210, 210},
                 {MapRegion::CommandHall, "예배당 진입", {0, -700}, 210, 210},
                 {MapRegion::Battlefield, "손상 성벽", {650, -700}, 210, 210},
                 {MapRegion::CommandHall, "ZONE-13 벽화 회랑", {0, -1250}, 280, 220},
                 {MapRegion::GuardTunnel, "ZONE-14 내부 회랑", {0, -1670}, 200, 180},
                 {MapRegion::Arena, "ZONE-15 용사 고정 전투장", {-500, -2100}, 300, 260},
                 {MapRegion::WorkTunnel, "ZONE-16 플레이어 우회로", {450, -2100}, 280, 260},
                 {MapRegion::Arena, "ZONE-17 교량 제어구역", {0, -2900}, 580, 420},
                 {MapRegion::Outpost, "ZONE-18 귀환로", {0, -3550}, 240, 200}};
    map.roads = {{{0, 0}, {-650, -700}, 100},
                 {{0, 0}, {0, -700}, 100},
                 {{0, 0}, {650, -700}, 100},
                 {{-650, -700}, {0, -1250}, 100},
                 {{0, -700}, {0, -1250}, 100},
                 {{650, -700}, {0, -1250}, 100},
                 {{0, -1250}, {0, -1670}, 105},
                 {{0, -1670}, {-500, -2100}, 100},
                 {{0, -1670}, {450, -2100}, 100},
                 {{-500, -2100}, {0, -2900}, 100},
                 {{450, -2100}, {0, -2900}, 100},
                 {{0, -2900}, {0, -3550}, 100}};
    // The signal stands between the two diagonal approaches, outside the boss arena.
    // Join both approaches with real floor so the A* goal is traversable.
    map.roads.push_back({{-300, -2500}, {300, -2500}, 100.f});
    map.interior = MapRectangle{{0, -1550}, 1100, 2300};
    map.wallName = "옛 성채 벽면";
    map.landmarks = {
        Site(Chapter2Objects::Aqueduct,
             {-650, -700},
             "급수로 진입 선택",
             "오래된 급수로를 통해 성채에 진입합니다."),
        Site(Chapter2Objects::Chapel,
             {0, -700},
             "예배당 진입 선택",
             "수용소를 먼저 처리해 얻은 통로 정보가 필요합니다."),
        Site(Chapter2Objects::Breach,
             {650, -700},
             "손상 성벽 진입 선택",
             "공성 작업으로 무너진 성벽 틈으로 진입합니다."),
        Site(
            Chapter2Objects::Mural,
            {0, -1250},
            "일곱 인물 벽화",
            "일곱 귀환 인물 중 한 자리의 얼굴·이름만 후대에 훼손됐습니다. 그 인물은 쓰러진 이종족 전사에게 손을 내밉니다. 탁본을 확보합니다.",
            LandmarkKind::Records),
        Site(Chapter2Objects::DemolitionLine,
             {450, -1920},
             "폭파선 차단",
             "우회문 장치와 처리 순서를 바꿔 확인할 수 있습니다."),
        Site(Chapter2Objects::Bypass,
             {450, -2270},
             "우회문 해제",
             "용사는 전투에서 지는 것이 아니라 지형과 병력에 붙잡혀 있습니다.",
             LandmarkKind::CommandGate),
        Site(Chapter2Objects::ReunionSignal,
             {0, -2470},
             "용사 합류 신호",
             "폭파선과 우회문 처리 후 용사가 포위선에서 이탈해 합류합니다."),
        Site(Chapter2Objects::RetreatChain,
             {-340, -2830},
             "퇴로 쇠사슬 차단",
             "플레이어의 입력으로 문을 내려 퇴로를 막습니다.",
             LandmarkKind::CommandGate),
        Site(Chapter2Objects::ArmorOpening,
             {0, -2730},
             "용사 압박 지점",
             "퇴로를 차단하면 용사가 바르칸의 자세를 무너뜨릴 기회를 만듭니다."),
        Site(Chapter2Objects::CommandTarget,
             {340, -2830},
             "지휘 인장",
             "용사의 자세 붕괴 후 플레이어가 최종 원인 입력을 해야 합니다.",
             LandmarkKind::Records),
        Site(
            Chapter2Objects::JointFinisher,
            {0, -3110},
            "교량 제어구역",
            "플레이어가 지휘 인장을 파괴하면 용사와 공동 마무리를 진행합니다. 교량·보급 일부 손실은 남습니다."),
        Site(Chapter2Objects::BossDemolition,
             {-300, -3040},
             "교량 폭파 장치",
             "Space 유지로 남은 폭파 신호를 차단합니다.",
             LandmarkKind::Workbench),
        Site(Chapter2Objects::AftermathExit,
             {0, -3550},
             "구호지 귀환",
             "바르칸 격파 후 손상된 교량을 건너 협곡 구호지로 돌아갑니다.")};
    map.encounters = {{0, -1550}, {-500, -2100}, {450, -2100}, {0, -2900}};
    return map;
}
