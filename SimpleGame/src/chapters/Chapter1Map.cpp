#include "stdafx.h"
#include "Chapter1.h"
#include <stdexcept>

static MapLandmark Landmark(LandmarkKind kind,
                            ObjectId object,
                            WorldPosition position,
                            const char* name,
                            const char* description,
                            float radius = 0.f)
{
    return {kind, position, name, description, radius, object};
}

void Chapter1::PopulateVillage(MapDefinition& layout)
{
    layout.areas = {{MapRegion::Village, "국경 마을 배급소", {0, 0}, 560.f, 430.f},
                    {MapRegion::Farmland, "습격당한 농경지", {0, -850}, 660.f, 560.f},
                    {MapRegion::Farmland, "북쪽 길목", {0, -1430}, 180.f, 150.f}};
    layout.roads = {{{0, 0}, {0, -350}, 92.f},
                    {{0, -350}, {0, -850}, 88.f},
                    {{0, -820}, {-420, -1030}, 64.f},
                    {{0, -820}, {400, -1050}, 64.f},
                    {{0, -1230}, {0, -1430}, 76.f}};
    layout.landmarks = {Landmark(LandmarkKind::Supplies,
                                 Chapter1Objects::Ration,
                                 {-150, -100},
                                 "배급 창구",
                                 "배급과 보수를 확인한다."),
                        Landmark(LandmarkKind::Supplies,
                                 Chapter1Objects::Donation,
                                 {130, -100},
                                 "구호 상자",
                                 "남는 식량을 주민에게 나눌 수 있다."),
                        Landmark(LandmarkKind::Supplies,
                                 Chapter1Objects::MilitarySupply,
                                 {300, -130},
                                 "봉인된 군 보급품",
                                 "주민용 배급품과 구분되어 있다."),
                        Landmark(LandmarkKind::Board,
                                 Chapter1Objects::Notice,
                                 {-300, 80},
                                 "징집·수확 공지",
                                 "전쟁 때문에 수확 인력이 부족하다는 공지다."),
                        Landmark(LandmarkKind::Farmhouse,
                                 Chapter1Objects::ClosedMill,
                                 {-390, -430},
                                 "닫힌 방앗간",
                                 "곡물은 있지만 방앗간은 멈춰 있다."),
                        Landmark(LandmarkKind::Veteran,
                                 Chapter1Objects::Farmer,
                                 {180, -520},
                                 "농부",
                                 "습격이 시작된 밭 가장자리를 가리킨다."),
                        Landmark(LandmarkKind::Supplies,
                                 Chapter1Objects::FoodEvidence,
                                 {-420, -1030},
                                 "사라진 식량 흔적",
                                 "곡물과 소금이 조직적으로 실려 나갔다."),
                        Landmark(LandmarkKind::Workbench,
                                 Chapter1Objects::RepairEvidence,
                                 {400, -1050},
                                 "수리 흔적",
                                 "연장과 숫돌이 함께 사라졌다."),
                        Landmark(LandmarkKind::Tracks,
                                 Chapter1Objects::TransportEvidence,
                                 {80, -1110},
                                 "운반 흔적",
                                 "여러 수레가 같은 북쪽 길을 이용했다."),
                        Landmark(LandmarkKind::Sign,
                                 Chapter1Objects::NorthExit,
                                 {0, -1430},
                                 "북쪽 숲길",
                                 "수레 흔적은 숲과 다리 쪽으로 이어진다.")};
    layout.encounters = {{0, -700}, {-260, -900}, {250, -930}};
}

void Chapter1::PopulateForest(MapDefinition& layout)
{
    layout.areas = {{MapRegion::Woods, "남쪽 숲길", {0, 0}, 300.f, 210.f},
                    {MapRegion::Woods, "수레 자국 갈림길", {0, -400}, 360.f, 230.f},
                    {MapRegion::Riverside, "얕은 개울", {260, -820}, 220.f, 170.f},
                    {MapRegion::Woods, "능선 전망", {450, -1220}, 310.f, 250.f},
                    {MapRegion::Battlefield, "목조 다리 호송대", {420, -1600}, 410.f, 270.f},
                    {MapRegion::MineEntrance, "폐광 방향 길", {360, -1930}, 180.f, 150.f}};
    layout.roads = {{{0, 0}, {0, -400}, 78.f},
                    {{0, -400}, {260, -650}, 68.f},
                    {{260, -650}, {260, -1000}, 58.f},
                    {{260, -1000}, {450, -1220}, 68.f},
                    {{450, -1220}, {420, -1600}, 76.f},
                    {{420, -1600}, {360, -1930}, 72.f},
                    {{0, -400}, {-500, -600}, 58.f},
                    {{-500, -600}, {-420, -970}, 54.f},
                    {{-420, -970}, {120, -1040}, 52.f}};
    layout.landmarks = {Landmark(LandmarkKind::Tracks,
                                 Chapter1Objects::LightTracks,
                                 {-110, -410},
                                 "가벼운 수레 자국",
                                 "가벼운 수레 자국은 서쪽의 미끼 길로 이어진다."),
                        Landmark(LandmarkKind::Tracks,
                                 Chapter1Objects::HeavyTracks,
                                 {150, -430},
                                 "깊은 수레 자국",
                                 "무거운 짐을 실은 수레가 북동쪽으로 향했다."),
                        Landmark(LandmarkKind::Supplies,
                                 Chapter1Objects::Grain,
                                 {220, -650},
                                 "흩어진 곡물",
                                 "비에 젖기 전의 곡물이 길을 가리킨다."),
                        Landmark(LandmarkKind::Sign,
                                 Chapter1Objects::Bark,
                                 {410, -1030},
                                 "긁힌 나무껍질",
                                 "무거운 수레가 회전하며 남긴 자국이다."),
                        Landmark(LandmarkKind::Tracks,
                                 Chapter1Objects::Rope,
                                 {480, -1120},
                                 "젖은 밧줄 섬유",
                                 "호송용 밧줄 조각이 능선 쪽에 남아 있다."),
                        Landmark(LandmarkKind::Banner,
                                 Chapter1Objects::Ridge,
                                 {450, -1220},
                                 "능선",
                                 "아래의 목조 다리와 호송대를 내려다볼 수 있다."),
                        Landmark(LandmarkKind::Wagon,
                                 Chapter1Objects::Wagon,
                                 {420, -1570},
                                 "군 보급 마차",
                                 "바퀴축을 멈추면 호송대를 저지할 수 있다."),
                        Landmark(LandmarkKind::Sign,
                                 Chapter1Objects::Prisoner,
                                 {220, -1660},
                                 "포로 수레",
                                 "묶인 주민들을 풀어 줄 수 있다."),
                        Landmark(LandmarkKind::Records,
                                 Chapter1Objects::Invoice,
                                 {610, -1660},
                                 "광산 표식 송장",
                                 "보급품의 목적지는 북쪽 폐광이다."),
                        Landmark(LandmarkKind::Sign,
                                 Chapter1Objects::MineExit,
                                 {360, -1930},
                                 "폐광 가는 길",
                                 "산길 끝에 버려진 폐광 기지가 있다.")};
    layout.encounters = {{-470, -690}, {420, -1510}, {300, -1690}};
}

void Chapter1::PopulateMine(MapDefinition& layout)
{
    layout.areas = {{MapRegion::MineEntrance, "폐광 정문", {0, -160}, 300.f, 220.f},
                    {MapRegion::GuardTunnel, "배수 갱도", {-760, -340}, 160.f, 220.f},
                    {MapRegion::SupplyStore, "중계 창고", {0, -700}, 310.f, 250.f},
                    {MapRegion::WorkTunnel, "훈련·수리 구역", {390, -970}, 250.f, 250.f},
                    {MapRegion::CentralMine, "승강기 깊이길", {0, -1270}, 250.f, 230.f},
                    {MapRegion::Arena, "지휘 전투 공간", {0, -2000}, 550.f, 460.f},
                    {MapRegion::CommandHall, "지휘실과 출구", {0, -2730}, 320.f, 320.f}};
    layout.roads = {{{0, 0}, {0, -700}, 62.f},
                    {{0, 0}, {-420, -120}, 62.f},
                    {{-420, -120}, {-760, -340}, 62.f},
                    {{-760, -340}, {-760, -920}, 56.f},
                    {{-760, -920}, {-270, -1040}, 56.f},
                    {{-270, -1040}, {0, -1270}, 58.f},
                    {{0, -700}, {390, -970}, 62.f},
                    {{0, -700}, {0, -1270}, 64.f},
                    {{0, -1270}, {0, -1560}, 64.f},
                    {{0, -2460}, {0, -2730}, 72.f}};
    layout.landmarks = {Landmark(LandmarkKind::MineGate,
                                 Chapter1Objects::FrontGate,
                                 {0, -160},
                                 "폐광 정문",
                                 "정문은 짧지만 경비가 두텁다."),
                        Landmark(LandmarkKind::Sign,
                                 Chapter1Objects::Drain,
                                 {-760, -340},
                                 "배수로",
                                 "우회로는 길지만 초기 경비가 적다."),
                        Landmark(LandmarkKind::Supplies,
                                 Chapter1Objects::Storage,
                                 {-150, -700},
                                 "중계 창고",
                                 "농가와 호송대에서 온 물자가 쌓여 있다."),
                        Landmark(LandmarkKind::Workbench,
                                 Chapter1Objects::Training,
                                 {390, -970},
                                 "훈련 구역",
                                 "고블린들이 전투와 운반을 연습한다."),
                        Landmark(LandmarkKind::Workbench,
                                 Chapter1Objects::Repair,
                                 {170, -880},
                                 "수리대",
                                 "전쟁 물자를 계속 손질하고 있다."),
                        Landmark(LandmarkKind::Banner,
                                 Chapter1Objects::ShiftBell,
                                 {-40, -1000},
                                 "교대 종",
                                 "정해진 교대 시간에 경비를 부르는 종이다."),
                        Landmark(LandmarkKind::Records,
                                 Chapter1Objects::TransportRecords,
                                 {-120, -1210},
                                 "운반 기록",
                                 "승강기와 보급 흐름이 지휘 구역으로 이어진다."),
                        Landmark(LandmarkKind::Banner,
                                 Chapter1Objects::CommandSeal,
                                 {130, -1250},
                                 "지휘 인장",
                                 "이 부대가 독자적으로 움직이지 않는다는 증거다."),
                        Landmark(LandmarkKind::Wagon,
                                 Chapter1Objects::Lift,
                                 {0, -1370},
                                 "광차 승강기",
                                 "깊은 지휘 구역으로 내려가는 승강기다."),
                        Landmark(LandmarkKind::Banner,
                                 Chapter1Objects::BellA,
                                 {-360, -1830},
                                 "명령 종 A",
                                 "첫 번째 지원 병력을 부르는 종이다."),
                        Landmark(LandmarkKind::Banner,
                                 Chapter1Objects::BellB,
                                 {360, -1830},
                                 "명령 종 B",
                                 "두 번째 지원 병력을 부르는 종이다."),
                        Landmark(LandmarkKind::Banner,
                                 Chapter1Objects::BellC,
                                 {0, -2260},
                                 "명령 종 C",
                                 "치유와 후속 지원을 부르는 종이다."),
                        Landmark(LandmarkKind::CommandGate,
                                 Chapter1Objects::ReinforcementGate,
                                 {0, -2460},
                                 "증원 차단문",
                                 "기계를 조작해 증원 병력을 가둘 수 있다."),
                        Landmark(LandmarkKind::Records,
                                 Chapter1Objects::Ledger,
                                 {-120, -2730},
                                 "지휘 장부",
                                 "그레이월 요새로 향하는 다음 보급 기록이다."),
                        Landmark(LandmarkKind::Veteran,
                                 Chapter1Objects::Hero,
                                 {140, -2640},
                                 "용사",
                                 "전투가 끝난 뒤 이곳에서 이야기를 나눈다."),
                        Landmark(LandmarkKind::Sign,
                                 Chapter1Objects::ChapterExit,
                                 {0, -2960},
                                 "북부 전선 길",
                                 "그레이월 요새 방향으로 이어지는 길이다.")};
    layout.encounters = {
        {0, -260}, {-760, -590}, {0, -780}, {390, -970}, {0, -2000}, {-260, -2100}, {260, -2100}};
}

void Chapter1::PopulateChapter2Route(MapDefinition& layout)
{
    layout.areas = {{MapRegion::Outpost, "북부 전선 길", {0, 0}, 220.f, 180.f},
                    {MapRegion::Outpost, "그레이월 방향 경계", {0, -1040}, 220.f, 180.f}};
    layout.roads = {{{0, 0}, {0, -1040}, 74.f}};
    layout.landmarks = {Landmark(LandmarkKind::Sign,
                                 Chapter1Objects::ChapterExit,
                                 {0, -1040},
                                 "그레이월 요새 경계",
                                 "장부가 가리킨 회색벽 요새로 이동한다. [Space]")};
    layout.encounters.clear();
}

MapDefinition Chapter1::BuildMap(MapId map) const
{
    MapDefinition layout;
    layout.id = map;
    layout.entry = {0.f, 0.f};
    switch (map)
    {
    case Chapter1Maps::Village:
        layout.title = "국경 마을과 농경지";
        PopulateVillage(layout);
        break;
    case Chapter1Maps::Forest:
        layout.title = "북쪽 숲길과 목조 다리";
        layout.water = {{{240.f, -825.f}, 540.f, 62.f}};
        PopulateForest(layout);
        break;
    case Chapter1Maps::Mine:
        layout.title = "버려진 폐광 기지";
        layout.interior = MapRectangle{{-125.f, -1500.f}, 825.f, 1600.f};
        layout.wallName = "폐광 벽면";
        PopulateMine(layout);
        break;
    case Chapter1Maps::Chapter2Route:
        layout.title = "북부 전선 길";
        PopulateChapter2Route(layout);
        break;
    default:
        throw std::invalid_argument("챕터 1에 등록되지 않은 지역입니다.");
    }
    return layout;
}

WorldPosition Chapter1::Arena()
{
    return {0.f, -2000.f};
}
