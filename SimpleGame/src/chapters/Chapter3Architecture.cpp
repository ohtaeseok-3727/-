#include "stdafx.h"
#include "Chapter3Architecture.h"
#include "Chapter3.h"
#include <algorithm>
#include <cmath>

namespace
{
    void Building(MapDefinition& map,
                  const char* name,
                  WorldPosition center,
                  float width,
                  float depth,
                  float height,
                  BuildingStyle style)
    {
        if (style == BuildingStyle::Wall && (width > 180.f || depth > 180.f))
        {
            const bool horizontal = width > depth;
            const float half = horizontal ? width : depth;
            const int count = static_cast<int>(std::ceil(half * 2.f / 320.f));
            const float segment = half / count;
            for (int i = 0; i < count; ++i)
            {
                auto p = center;
                (horizontal ? p.x : p.y) += -half + segment * (2.f * i + 1.f);
                Building(map,
                         name,
                         p,
                         horizontal ? segment : width,
                         horizontal ? depth : segment,
                         height,
                         style);
            }
            return;
        }
        map.buildings.push_back({static_cast<int>(map.buildings.size()) + 1,
                                 name,
                                 {center, width, depth},
                                 height,
                                 style});
    }

    void Portal(MapDefinition& map,
                ObjectId id,
                WorldPosition p,
                const char* name,
                MapId target,
                WorldPosition arrival)
    {
        map.landmarks.push_back(
            {LandmarkKind::MineGate, p, name, "[Space] 기록관 포탈로 이동합니다.", 0.f, id});
        map.portals.push_back({id, target, arrival});
    }
}

void Chapter3Architecture::DressCity(MapDefinition& map)
{
    using namespace Chapter3Objects;
    // Move actual records into a separate public archive room, never into a facade collider.
    map.landmarks.erase(std::remove_if(map.landmarks.begin(),
                                       map.landmarks.end(),
                                       [](const MapLandmark& site)
                                       {
                                           return (site.object >= ManuscriptEarly &&
                                                   site.object <= ManuscriptLate) ||
                                                  site.object == Censure;
                                       }),
                        map.landmarks.end());
    map.areas = {{MapRegion::Village, "기록도시 아르겐", {0, -1500}, 1450, 2000},
                 {MapRegion::Village, "아르겐 남문 앞 안전 통행로", {0, 650}, 250, 150}};
    map.roads = {{{0, 200}, {0, -2900}, 140},
                 {{0, -650}, {-1250, -650}, 95},
                 {{-1250, -650}, {-1250, -1450}, 95},
                 {{-1250, -1450}, {-750, -1450}, 85},
                 {{-750, -1450}, {-750, -1750}, 95},
                 {{-750, -1750}, {0, -2050}, 95},
                 {{0, -650}, {700, -1050}, 110},
                 {{700, -1050}, {700, -1750}, 110},
                 {{700, -1750}, {0, -2050}, 110},
                 {{360, -250}, {360, -650}, 90},
                 {{360, -650}, {-360, -650}, 90},
                 {{-360, -650}, {-360, -250}, 90},
                 {{-360, -250}, {360, -250}, 90}};
    map.districts = {{"아르겐 · 남문과 행정 광장", {{0, -50}, 1400, 450}, 1},
                     {"아르겐 · 시장 거리와 행정 수로", {{0, -575}, 1400, 125}, 2},
                     {"아르겐 · 왕립 대서고 앞 기록 거리", {{-750, -1075}, 650, 375}, 1},
                     {"아르겐 · 영웅 기념묘와 석공 마당", {{750, -1000}, 650, 300}, 3},
                     {"아르겐 · 폐쇄 수도원 외벽 조사로", {{750, -1650}, 650, 350}, 3},
                     {"아르겐 · 추방민 공방 거리", {{-750, -1725}, 650, 275}, 2},
                     {"아르겐 · 시민 회랑과 왕실 연락소", {{0, -2550}, 1400, 550}, 1},
                     {"아르겐 · 기록관 봉인동 앞마당", {{0, -3300}, 1400, 200}, 1},
                     {"아르겐 · 도시 안 연결 거리", {{0, -1500}, 1450, 2000}, 1},
                     {"아르겐 · 남문 밖 진입로", {{0, 650}, 250, 150}, 1}};
    Building(map, "왕립 대서고", {-750, -1050}, 360, 300, 240, BuildingStyle::Archive);
    Building(map, "영웅 기념홀 · 기록관 별관", {-700, 170}, 250, 140, 200, BuildingStyle::Archive);
    Building(map, "봉인 서고 · 기록관 봉인동", {0, -3220}, 460, 180, 230, BuildingStyle::Archive);
    Building(
        map, "폐쇄 수도원 · 출입 불가", {1200, -1650}, 180, 340, 235, BuildingStyle::Monastery);
    Building(
        map, "석공 공방 · 외부 장부 창구", {1180, -1080}, 130, 150, 120, BuildingStyle::Workshop);
    Building(map, "시장 주택 · 출입 불가", {-620, -400}, 140, 130, 145, BuildingStyle::House);
    Building(map, "상인 주택 · 출입 불가", {650, -400}, 140, 130, 155, BuildingStyle::House);
    Building(
        map, "공동체 공방 · 외부 접견", {-1250, -1710}, 110, 160, 120, BuildingStyle::Workshop);
    Building(map, "왕실 연락소 · 외부 창구", {1030, -2800}, 240, 130, 170, BuildingStyle::House);
    // Individual wall objects close the city perimeter except the 400-unit southern gate.
    Building(map, "서쪽 성벽", {-1450, -1500}, 30, 2000, 105, BuildingStyle::Wall);
    Building(map, "동쪽 성벽", {1450, -1500}, 30, 2000, 105, BuildingStyle::Wall);
    Building(map, "북쪽 성벽", {0, -3500}, 1450, 30, 105, BuildingStyle::Wall);
    Building(map, "남문 서쪽 성벽", {-825, 500}, 625, 30, 105, BuildingStyle::Wall);
    Building(map, "남문 동쪽 성벽", {825, 500}, 625, 30, 105, BuildingStyle::Wall);
    map.roads.push_back({{0, 650}, {0, 200}, 150});
    map.roads.push_back({{0, 0}, {-350, 0}, 95});
    map.roads.push_back({{-350, 0}, {-350, 400}, 95});
    map.roads.push_back({{-350, 400}, {-700, 400}, 75});
    map.roads.push_back({{0, -650}, {-750, -650}, 95});
    map.roads.push_back({{-750, -1450}, {-1050, -1450}, 90});
    map.roads.push_back({{-1050, -1450}, {-1050, -2050}, 90});
    map.water.push_back({{1300, -600}, 65, 500});
    Portal(map,
           MemorialEntrance,
           {-700, 400},
           "기록관 별관 포탈 · 영웅 기념홀",
           Chapter3Maps::Memorial,
           {0, 180});
    Portal(map,
           ReadingEntrance,
           {-750, -650},
           "왕립 대서고 포탈 · 공개 열람실",
           Chapter3Maps::ReadingRoom,
           {0, 180});
    map.portals.push_back({ArchiveExit, Chapter3Maps::Archive, {0, 180}});
    for (auto& site : map.landmarks)
    {
        if (site.object == ArchiveExit)
        {
            site.name = "기록관 봉인동 포탈 · 옛 군용 통로";
        }
        else if (site.object == RestrictedDoor)
        {
            site.position = {-750, -1450};
        }
        else if (site.object == RefugeRoom)
        {
            site.name = "수도원 외벽 기록 · 입소실 배치";
        }
        else if (site.object == FuneralRegister)
        {
            site.name = "수도원 외벽 기록 · 장례 명부";
        }
        else if (site.object == LastSermon)
        {
            site.name = "수도원 외벽 기록 · 마지막 설교실";
        }
    }
    for (const auto site :
         {MapLandmark{LandmarkKind::Sign,
                      {0, 350},
                      "아르겐 남문",
                      "성벽 안은 기록도시입니다. 기록관만 포탈로 출입할 수 있습니다.",
                      0.f,
                      CityGate},
          MapLandmark{LandmarkKind::Sign,
                      {0, -450},
                      "시장 거리",
                      "서쪽은 대서고, 동쪽은 기념묘. 포장 도로와 골목으로 이동하세요.",
                      0.f,
                      MarketSign},
          MapLandmark{LandmarkKind::Sign,
                      {350, -1050},
                      "기념묘 조사 마당",
                      "석상 기초와 야외 공방 장부를 대조합니다. 공방 내부 출입은 불가합니다.",
                      0.f,
                      MemorialSign},
          MapLandmark{
              LandmarkKind::Sign,
              {650, -1350},
              "폐쇄 수도원 외벽 조사로",
              "수도원 문은 봉쇄되어 있습니다. 외벽의 방 배치와 이관 기록을 따라 조사하세요.",
              0.f,
              MonasterySign}})
    {
        map.landmarks.push_back(site);
    }
}

void Chapter3Architecture::EncloseRoom(MapDefinition& map, MapRectangle bounds)
{
    map.portalInterior = true;
    map.interior = bounds;
    map.districts.push_back({map.title, bounds, 1});
    Building(map,
             "기록관 서벽",
             {bounds.center.x - bounds.halfWidth, bounds.center.y},
             18,
             bounds.halfHeight + 18,
             90,
             BuildingStyle::Wall);
    Building(map,
             "기록관 동벽",
             {bounds.center.x + bounds.halfWidth, bounds.center.y},
             18,
             bounds.halfHeight + 18,
             90,
             BuildingStyle::Wall);
    Building(map,
             "기록관 북벽",
             {bounds.center.x, bounds.center.y - bounds.halfHeight},
             bounds.halfWidth,
             18,
             90,
             BuildingStyle::Wall);
    Building(map,
             "기록관 남벽",
             {bounds.center.x, bounds.center.y + bounds.halfHeight},
             bounds.halfWidth,
             18,
             90,
             BuildingStyle::Wall);
}

void Chapter3Architecture::DressReadingRoom(MapDefinition& map)
{
    EncloseRoom(map, {{0, -650}, 850, 1050});
    for (float x : {-630.f, 630.f})
    {
        for (float y : {-300.f, -700.f, -1100.f})
        {
            Building(map, "분리 서가", {x, y}, 80, 125, 125, BuildingStyle::Bookshelf);
        }
    }
}
