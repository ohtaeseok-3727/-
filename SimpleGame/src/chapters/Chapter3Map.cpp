#include "stdafx.h"
#include "Chapter3.h"
#include "Chapter3Architecture.h"
#include <stdexcept>

namespace
{
    MapLandmark Site(ObjectId id,
                     WorldPosition p,
                     const char* name,
                     const char* description,
                     LandmarkKind kind = LandmarkKind::Records)
    {
        return {kind, p, name, description, 0.f, id};
    }
}

MapDefinition Chapter3::BuildMap(MapId map) const
{
    switch (map)
    {
    case Chapter3Maps::Memorial:
        return MemorialMap();
    case Chapter3Maps::Argen:
        return CityMap();
    case Chapter3Maps::Archive:
        return ArchiveMap();
    case Chapter3Maps::ReadingRoom:
        return ReadingRoomMap();
    default:
        throw std::invalid_argument("챕터 3에 등록되지 않은 지역입니다.");
    }
}

MapDefinition Chapter3::MemorialMap()
{
    using namespace Chapter3Objects;
    MapDefinition map;
    map.id = Chapter3Maps::Memorial;
    map.title = "왕립 기록관 · 일곱 번째 불";
    map.entry = {0, 180};
    map.areas = {{MapRegion::Village, "영웅 기념홀", {0, -350}, 600, 800}};
    map.roads = {{{0, 180}, {0, -900}, 140}};
    map.landmarks = {
        Site(
            Ceremony,
            {0, -520},
            "일곱 개의 불",
            "의식에서 일곱 불이 켜졌지만 여섯 이름만 낭독된다. 마지막 불은 '이름 없는 동료'에게 바쳐졌다."),
        Site(
            Plaque,
            {-240, -250},
            "여섯 이름의 명판",
            "용사: 나도 일곱이라고 배우고 여섯 이름만 외웠어. 이름을 버린 게 아니라 빼앗긴 거라면?"),
        Site(
            Rubbing,
            {240, -250},
            "엘리안의 탁본 대조",
            "엘리안: 훼손은 원 안료보다 후대입니다. 플레이어: 스스로 이름을 버린 사람의 얼굴을 누가 긁어내지?"),
        Site(CityExit,
             {0, -900},
             "아르겐으로",
             "오래된 원본을 찾아 기록도시로 이동합니다.",
             LandmarkKind::Sign)};
    map.portals.push_back({CityExit, Chapter3Maps::Argen, {-700, 400}});
    Chapter3Architecture::EncloseRoom(map, {{0, -350}, 600, 800});
    return map;
}

WorldPosition Chapter3::Foundation(int index)
{
    return {500.f + 200.f * index, -900.f};
}

MapDefinition Chapter3::CityMap()
{
    using namespace Chapter3Objects;
    MapDefinition map;
    map.id = Chapter3Maps::Argen;
    map.title = "기록도시 아르겐";
    map.entry = {0, 200};
    // One safe civic area prevents procedural monster packs on the investigation routes.
    map.areas = {{MapRegion::Village, "아르겐 · 행정과 기록의 도시", {0, -1350}, 1250, 1800}};
    map.roads = {{{0, 200}, {0, -2900}, 140},
                 {{0, -650}, {-750, -1050}, 130},
                 {{-750, -1050}, {-750, -1750}, 130},
                 {{0, -650}, {700, -1050}, 130},
                 {{700, -1050}, {700, -1750}, 130},
                 {{-750, -1750}, {0, -2050}, 130},
                 {{700, -1750}, {0, -2050}, 130},
                 {{360, -250}, {360, -650}, 100},
                 {{360, -650}, {-360, -650}, 100},
                 {{-360, -650}, {-360, -250}, 100},
                 {{-360, -250}, {360, -250}, 100}};
    map.landmarks = {
        Site(
            Petition,
            {-260, 80},
            "전사자 유족과 거주권 민원",
            "명부 누락으로 연금이 끊긴 유족, 탈영 기록으로 거주권이 정지된 가족. 광장에서는 영웅 이름을 걸고 징집 선서가 열린다."),
        Site(Rations,
             {260, 80},
             "동부 피난민 배급소",
             "협곡에서 살아온 사람들과 남겨진 피해를 확인합니다.",
             LandmarkKind::Supplies),
        Site(Permit,
             {0, -250},
             "열람 허가 창구",
             "행정 광장과 배급소를 살펴본 뒤 조사 허가를 받습니다.",
             LandmarkKind::Board),
        Site(Shortcut,
             {0, -650},
             "시장 지름길",
             "연락책이 도는 시장 골목을 가로질러 앞을 막습니다.",
             LandmarkKind::Tracks),
        Site(ManuscriptEarly,
             {-950, -900},
             "초판 · 일곱 이름",
             "초판: 일곱 인물의 이름과 숫자 '일곱'이 함께 남아 있다."),
        Site(ManuscriptMiddle,
             {-550, -900},
             "중기 개정본 · 지운 이름",
             "중기 개정본: 숫자 일곱은 남고 한 이름이 사라졌다."),
        Site(
            ManuscriptLate,
            {-750, -1150},
            "후대 재필사본 · 재점화 방지",
            "후대 재필사본: 같은 삭제가 반복됐다. 결재 문구 '재점화 방지'는 확인되지만 최종 목적은 미확인이다."),
        Site(FoundationWest,
             Foundation(0),
             "기초 서쪽 · 석재 대조",
             "Space 유지: 남은 석상과 철거 흔적의 규격을 대조합니다.",
             LandmarkKind::Workbench),
        Site(FoundationCenter,
             Foundation(1),
             "기초 중앙 · 홈 복원",
             "Space 유지: 일곱째 석상의 고정 홈을 맞춥니다.",
             LandmarkKind::Workbench),
        Site(FoundationEast,
             Foundation(2),
             "기초 동쪽 · 접합 복원",
             "Space 유지: 같은 시기에 놓인 접합부를 확인합니다.",
             LandmarkKind::Workbench),
        Site(WorkshopLedger,
             {700, -1150},
             "석공 공방 장부",
             "세 기초를 복원한 뒤 일곱 석상의 제작 기록과 대조합니다."),
        Site(RefugeRoom,
             {500, -1450},
             "수도원 입소실 · 초기 필체",
             "초기: 이종족 피난민을 성 안에 받아들인다. 같은 인장을 따라 장례 명부로 이동하세요."),
        Site(FuneralRegister,
             {900, -1600},
             "수도원 장례 명부 · 중기 필체",
             "중기: 인간은 인간의 힘으로 지켜야 한다. 같은 필체가 마지막 설교실로 이어집니다."),
        Site(
            LastSermon,
            {600, -1850},
            "마지막 설교실 · 후기 필체",
            "후기: 공존은 인간을 파멸시킨다. 같은 사람의 변화다. 억울한 성인이나 처음부터 악인으로 단정하지 않는다."),
        Site(Censure,
             {0, -1300},
             "세라드의 회수 통보",
             "세 조사 결과를 등록한 뒤 기록수호대장의 통보를 확인합니다."),
        Site(Edric,
             {-900, -1650},
             "추방민 작업장 · 에드릭",
             "연락 표식을 제시하고 절도에 대한 책임과 기록의 출처를 묻습니다."),
        Site(TransferSeal,
             {-600, -1850},
             "작전 인장 대조",
             "에드릭의 탁본과 수도원 기록의 인장을 대조합니다."),
        Site(ArchiveExit,
             {0, -2900},
             "옛 군용 통로",
             "오늘 밤 원본 이송 전 봉인 서고에 진입합니다.",
             LandmarkKind::MineGate),
        Site(
            ConfirmedNotice,
            {-280, -2050},
            "공개문 · 확인된 사실",
            "확인: 일곱째의 실존, 반복된 삭제, 말년의 배타적 사상, 전쟁 전 인간군 원정. 엘리안이 시민·성직자·기록관 직원 앞에서 원본 목록과 진위를 낭독한다."),
        Site(
            OpenNotice,
            {280, -2050},
            "공개문 · 아직 열린 질문",
            "미확인: 일곱째의 이름, 삭제 최초 명령자, 다른 여섯 영웅의 입장, 원정 목적·결과, 전쟁의 발단. 빈칸을 추측으로 채우지 않는다."),
        Site(CopyCommunity,
             {-600, -2150},
             "공증 사본 · 에드릭 공동체",
             "독립 보관처 1: 에드릭 공동체."),
        Site(CopySurvivors,
             {600, -2150},
             "공증 사본 · 회색벽 생존자",
             "독립 보관처 2: 회색벽 생존자 대표."),
        Site(CopyArchive,
             {0, -2250},
             "공증 사본 · 기록관 분산 서고",
             "독립 보관처 3: 왕립 기록관 내부의 서로 다른 서고."),
        Site(ExtraClergy,
             {-550, -2450},
             "추가 보관처 선택 · 성직자",
             "추가 사본 한 부를 성직자 공동체에 맡깁니다."),
        Site(ExtraGuild,
             {550, -2450},
             "추가 보관처 선택 · 상인 조합",
             "추가 사본 한 부를 상인 조합에 맡깁니다."),
        Site(Testimony,
             {0, -1900},
             "용사의 증언 인장",
             "사본이 출발한 뒤 용사가 원본의 존재와 직접 확인을 증언합니다.",
             LandmarkKind::Banner),
        Site(Hearing,
             {0, -2550},
             "왕실 연락소 · 심의 통보",
             "정치적 조치와 제한 현장 조사 조건을 확인합니다.",
             LandmarkKind::Board),
        Site(RestrictedDoor,
             {-1050, -1200},
             "기록관 출입 제한",
             "봉인된 기록관 문: 일부 원본 열람 중지. 심의 기간에는 추가 기록 접근이 제한됩니다.",
             LandmarkKind::CommandGate),
        Site(ElianDesk,
             {-1050, -1450},
             "엘리안의 빈 책상",
             "책상에서 명패가 내려갔다. 엘리안은 해임됐지만 원본 검증 증언은 남는다."),
        Site(SealedWagon,
             {-1050, -1950},
             "에드릭행 보급 수레",
             "수레에 왕실 봉인이 붙었다. 에드릭 공동체로 향하는 보급이 제재된다.",
             LandmarkKind::Wagon),
        Site(SupplyDesk,
             {900, -2450},
             "제한 보급 창구",
             "장비·호송은 동결. 치료품과 최소 여비만 1회 지급할 수 있습니다.",
             LandmarkKind::Supplies),
        Site(
            CommandSeal,
            {300, -2700},
            "용사의 현장 지휘 인장",
            "군용 통신 인장·현장 긴급 지휘 인장은 유효하다. 용사의 공식 직위와 현장 권한은 유지된다.",
            LandmarkKind::Banner),
        Site(
            FieldPermit,
            {-300, -2700},
            "원정 좌표와 제한 조사 승인서",
            "왕실 심의 아래 추가 증거를 확보한다. 야수족 영역에서 인간 기록과 독립 현장 증거를 대조하자.",
            LandmarkKind::Sign)};
    Chapter3Architecture::DressCity(map);
    return map;
}

MapDefinition Chapter3::ArchiveMap()
{
    using namespace Chapter3Objects;
    MapDefinition map;
    map.id = Chapter3Maps::Archive;
    map.title = "봉인 서고 · 원본 반출 저지";
    map.entry = {0, 180};
    map.interior = MapRectangle{{0, -1000}, 1050, 1500};
    map.wallName = "서고 외벽";
    map.areas = {{MapRegion::GuardTunnel, "하부 군용 통로", {0, -200}, 180, 480},
                 {MapRegion::SupplyStore, "압수 창고", {0, -850}, 650, 320},
                 {MapRegion::CommandHall, "중앙 회랑", {0, -1400}, 650, 350},
                 {MapRegion::Arena, "수도행 기록 승강대", {0, -1900}, 650, 260}};
    map.roads = {{{0, 180}, {0, -2000}, 170},
                 {{0, -150}, {-500, -150}, 95},
                 {{-500, -150}, {-500, -900}, 95},
                 {{0, -2000}, {-850, -2000}, 90},
                 {{-850, -2000}, {-850, 180}, 90},
                 {{-850, 180}, {0, 180}, 90}};
    map.landmarks = {
        Site(MovementList,
             {-500, -650},
             "압수 상자 이동표",
             "북부 경계 정화 작전 · 제7부대·정규군 · 보급 72상자."),
        Site(StorageDate,
             {500, -650},
             "보관 서고 연대",
             "최초 아크 드래곤 교전 이전에 작성된 원본을 찾습니다."),
        Site(WrongDateBox,
             {-400, -950},
             "상자 A · 교전 이후 / 72",
             "보급 수량은 같지만 최초 교전 이후 기록이다."),
        Site(WrongSupplyBox,
             {400, -950},
             "상자 B · 교전 이전 / 27",
             "연대는 맞지만 부대 표식과 보급 수량이 다르다."),
        Site(OriginalBox,
             {0, -950},
             "상자 C · 교전 이전 / 72",
             "인간 정규군과 일곱째 소속 군대의 북부 이동 경로. 목적과 최종 명령은 훼손됐다."),
        Site(Brake,
             {350, -1800},
             "승강 플랫폼 브레이크",
             "바퀴 고정장치를 끊은 뒤 Space를 유지해 최종 반출을 막습니다.",
             LandmarkKind::Workbench),
        Site(
            Disarm,
            {-350, -1500},
            "비살상 교전 안내",
            "Ctrl·스킬로 기력 소진. 공격 직전 Space를 새로 눌러 패링. 반출을 막은 뒤 세라드 곁에서 Space 유지.",
            LandmarkKind::Sign),
        Site(RecoverOriginal,
             {0, -2050},
             "원본 회수 지점",
             "세라드 생존 제압과 승강대 정지 후 원본을 회수합니다."),
        Site(CityReturn,
             {-850, -2000},
             "아르겐 귀환로",
             "서쪽 통로를 따라 아르겐 시민 회랑으로 돌아갑니다.",
             LandmarkKind::Sign)};
    map.landmarks.push_back(Site(ArchiveReturn,
                                 {0, 280},
                                 "기록관 포탈 · 거리로 돌아가기",
                                 "아르겐 봉인동 앞마당으로 돌아갑니다.",
                                 LandmarkKind::MineGate));
    map.portals = {{ArchiveReturn, Chapter3Maps::Argen, {0, -2820}},
                   {CityReturn, Chapter3Maps::Argen, {0, -2050}}};
    Chapter3Architecture::EncloseRoom(map, {{0, -1000}, 1050, 1500});
    return map;
}

MapDefinition Chapter3::ReadingRoomMap()
{
    using namespace Chapter3Objects;
    MapDefinition map;
    map.id = Chapter3Maps::ReadingRoom;
    map.title = "왕립 대서고 · 공개 열람실";
    map.entry = {0, 180};
    map.areas = {{MapRegion::Village, "열람실과 중앙 회랑", {0, -650}, 850, 1050}};
    map.roads = {{{0, 260}, {0, -1400}, 130}, {{-400, -450}, {400, -450}, 100}};
    map.landmarks = {
        Site(ManuscriptEarly,
             {-300, -450},
             "초판 · 일곱 이름",
             "초판: 일곱 인물의 이름과 숫자 '일곱'이 함께 남아 있다."),
        Site(ManuscriptMiddle,
             {300, -450},
             "중기 개정본 · 지운 이름",
             "중기 개정본: 숫자 일곱은 남고 한 이름이 사라졌다."),
        Site(ManuscriptLate,
             {0, -850},
             "후대 재필사본 · 재점화 방지",
             "같은 삭제가 반복된다. 결재 문구 '재점화 방지'는 확인되지만 최종 목적은 미확인이다."),
        Site(Censure,
             {0, -1200},
             "중앙 회랑 · 세라드의 회수 통보",
             "세 조사 결과를 등록한 뒤 기록수호대장의 통보를 확인합니다."),
        Site(ReadingReturn,
             {0, 280},
             "기록관 포탈 · 기록 거리로",
             "공개 열람실을 나가 아르겐 기록 거리로 돌아갑니다.",
             LandmarkKind::MineGate)};
    map.portals = {{ReadingReturn, Chapter3Maps::Argen, {-750, -590}}};
    Chapter3Architecture::DressReadingRoom(map);
    return map;
}
