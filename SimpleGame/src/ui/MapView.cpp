#include "stdafx.h"
#include "MapView.h"
#include <algorithm>
#include <cmath>
#include <string>

void MapView::DrawTile(Renderer& r, const LevelMap& map, int x, int y, Point p) const
{
    const auto& layout = map.Layout();
    const float shade = float(map.Hash(x, y) % 17) / 1000.f;
    // Small ground cells keep narrow mine passages and curved bridges visible.
    for (int row = 0; row < 4; ++row)
    {
        for (int col = 0; col < 4; ++col)
        {
            const WorldPosition world{x * 64.f + col * 16.f + 8.f, y * 64.f + row * 16.f + 8.f};
            const auto region = layout.RegionAt(world);
            const bool mine = layout.Interior(world);
            const bool water = layout.Water(world);
            const bool road = map.IsRoad(world.x, world.y);
            Color color(.13f + shade, .18f + shade, .15f + shade);
            if (region == MapRegion::Farmland)
            {
                color = Color(.26f + shade, .25f + shade, .16f + shade);
            }
            else if (region == MapRegion::Woods || region == MapRegion::Outpost)
            {
                color = Color(.10f + shade, .15f + shade, .12f + shade);
            }
            else if (region == MapRegion::Battlefield)
            {
                color = Color(.20f + shade, .18f + shade, .16f + shade);
            }
            else if (region == MapRegion::MineEntrance)
            {
                color = Color(.23f + shade, .23f + shade, .22f + shade);
            }
            if (water)
            {
                color = Color(.10f + shade, .26f + shade, .32f + shade);
            }
            if (road)
            {
                color = Color(.30f + shade, .27f + shade, .21f + shade);
            }
            if (mine)
            {
                color = layout.InteriorFloor(world)
                            ? Color(.24f + shade, .23f + shade, .21f + shade)
                            : Color(.045f, .055f, .065f);
            }
            const float px = p.x + col * 16.f;
            const float py = p.y + row * 16.f * .8660254f;
            r.Rect(px, py, 16.5f, 14.4f, color);
            if (!mine && region == MapRegion::Farmland && !road)
            {
                r.Rect(px + 2, py + 4, 12, 2, Color(.16f, .19f, .10f));
                r.Rect(px + 4, py + 3, 3, 3, Color(.43f, .44f, .19f));
            }
            if (water && road)
            {
                r.Rect(px, py, 16, 2, Color(.47f, .36f, .23f));
            }
            if (!mine && region == MapRegion::Battlefield && !road &&
                map.Hash(x * 4 + col, y * 4 + row, 5) % 13 == 0)
            {
                r.Rect(px + 2, py + 3, 12, 5, Color(.10f, .11f, .10f));
            }
        }
    }
}

void MapView::DrawLandmark(Renderer& r, const MapLandmark& site, Point p, float animationTime) const
{
    const auto kind = site.kind;
    const Color wood(.38f, .27f, .17f);
    const Color mark(.75f, .31f, .19f);
    if (kind == LandmarkKind::Shop || kind == LandmarkKind::Trader || kind == LandmarkKind::Inn ||
        kind == LandmarkKind::Farmhouse)
    {
        const bool abandoned = kind == LandmarkKind::Farmhouse;
        r.Rect(p.x - 30, p.y - 56, 60, 65, Color(.37f, .33f, .26f));
        r.Triangle({p.x - 39, p.y - 51},
                   {p.x, p.y - 87},
                   {p.x + 39, p.y - 51},
                   abandoned ? Color(.23f, .21f, .17f) : Color(.37f, .18f, .15f));
        r.Rect(p.x - 9, p.y - 17, 18, 26, Color(.10f, .09f, .08f));
        r.Rect(p.x - 23,
               p.y - 40,
               12,
               14,
               abandoned ? Color(.12f, .13f, .10f) : Color(.88f, .64f, .27f));
        r.Rect(p.x + 12, p.y - 40, 12, 14, Color(.29f, .32f, .27f));
        if (abandoned)
        {
            r.Rect(p.x + 40, p.y - 3, 16, 8, wood);
            r.Ellipse(p.x - 43, p.y + 8, 3, 2, Color(.85f, .69f, .22f));
        }
    }
    else if (kind == LandmarkKind::Wagon || kind == LandmarkKind::Workbench)
    {
        r.Rect(p.x - 26, p.y - 22, 52, 18, wood);
        r.Ellipse(p.x - 21, p.y - 2, 9, 9, Color(.17f, .14f, .10f));
        r.Ellipse(p.x + 23, p.y + 3, 9, 6, Color(.17f, .14f, .10f));
        r.Rect(p.x - 12, p.y - 30, 6, 26, Color(.53f, .40f, .25f));
        r.Rect(p.x + 2, p.y - 29, 22, 4, Color(.62f, .59f, .48f));
    }
    else if (kind == LandmarkKind::Supplies)
    {
        for (int i = 0; i < 3; ++i)
        {
            const float px = p.x - 25 + i * 18.f;
            const float py = p.y - 16 - (i % 2) * 13.f;
            r.Rect(px, py, 20, 22, wood);
            r.Rect(px + 2, py + 3, 16, 3, Color(.61f, .46f, .28f));
        }
    }
    else if (kind == LandmarkKind::Tracks)
    {
        for (int i = 0; i < 5; ++i)
        {
            r.Rect(p.x - 32 + i * 14.f, p.y - i * 4.f, 9, 3, wood);
            r.Rect(p.x - 32 + i * 14.f, p.y + 13 - i * 4.f, 9, 3, wood);
        }
    }
    else if (kind == LandmarkKind::MineGate || kind == LandmarkKind::CommandGate ||
             kind == LandmarkKind::Palisade)
    {
        // Open central gap: decorative gate columns do not create a locked transition.
        for (int side : {-1, 1})
        {
            r.Rect(p.x + side * 66 - 8, p.y - 74, 16, 84, wood);
            r.Triangle({p.x + side * 66 - 8, p.y - 74},
                       {p.x + side * 66, p.y - 88},
                       {p.x + side * 66 + 8, p.y - 74},
                       wood);
        }
        r.Rect(p.x - 74, p.y - 75, 148, 12, wood);
    }
    else if (kind == LandmarkKind::Veteran)
    {
        const int frame = static_cast<int>(std::fmod(animationTime * 3.f, 4.f));
        r.VeteranMercenary(p.x, p.y, frame);
    }
    else if (kind == LandmarkKind::Banner || kind == LandmarkKind::Arena)
    {
        r.Rect(p.x - 2, p.y - 60, 4, 60, wood);
        r.Quad({p.x + 2, p.y - 60},
               {p.x + 29, p.y - 57},
               {p.x + 24, p.y - 29},
               {p.x + 2, p.y - 33},
               mark);
        r.Rect(p.x + 11, p.y - 51, 4, 14, Color(.85f, .70f, .40f));
    }
    else
    {
        r.Rect(p.x - 20, p.y - 45, 40, 29, wood);
        r.Rect(p.x - 15, p.y - 40, 30, 18, Color(.77f, .70f, .49f));
        r.Rect(p.x - 15, p.y - 16, 4, 16, wood);
        r.Rect(p.x + 11, p.y - 16, 4, 16, wood);
    }
}

float MapView::BubbleAnchorHeight(LandmarkKind kind) const
{
    switch (kind)
    {
    case LandmarkKind::Shop:
    case LandmarkKind::Trader:
    case LandmarkKind::Inn:
    case LandmarkKind::Farmhouse:
    case LandmarkKind::MineGate:
    case LandmarkKind::CommandGate:
    case LandmarkKind::Palisade:
        return 96.f;
    case LandmarkKind::Veteran:
        return 58.f;
    case LandmarkKind::Banner:
    case LandmarkKind::Arena:
        return 70.f;
    case LandmarkKind::Wagon:
    case LandmarkKind::Workbench:
        return 42.f;
    case LandmarkKind::Supplies:
    case LandmarkKind::Tracks:
        return 36.f;
    default:
        return 58.f;
    }
}

void MapView::DrawOverview(
    Renderer& r, const LevelMap& map, WorldPosition player, int width, int height) const
{
    if (!m_OverviewOpen)
    {
        return;
    }
    const float panelWidth = (std::min)(float(width - 32), 1080.f);
    const float panelHeight = (std::min)(float(height - 150), 560.f);
    if (panelWidth < 520 || panelHeight < 370)
    {
        r.Text(20, 150, "경로 지도를 보려면 창을 키우세요. [M]", Color(1.f, .8f, .5f));
        return;
    }
    const float x = (width - panelWidth) * .5f;
    const float y = 128.f;
    r.Rect(x, y, panelWidth, panelHeight, Color(.025f, .045f, .052f, .98f));
    const auto& title = map.Layout().Definition().title;
    r.Text(x + 18,
           y + 25,
           std::string(title) + " [M]   Space: 주변 지점 확인",
           Color(.91f, .83f, .64f));

    const auto& layout = map.Layout();
    const auto& areas = layout.Areas();
    const auto& roads = layout.Roads();
    float minX = player.x;
    float maxX = player.x;
    float minY = player.y;
    float maxY = player.y;
    const auto include = [&](WorldPosition p, float padding)
    {
        minX = (std::min)(minX, p.x - padding);
        maxX = (std::max)(maxX, p.x + padding);
        minY = (std::min)(minY, p.y - padding);
        maxY = (std::max)(maxY, p.y + padding);
    };
    for (const auto& area : areas)
    {
        include(area.center, (std::max)(area.halfWidth, area.halfHeight));
    }
    for (const auto& road : roads)
    {
        include(road.from, road.halfWidth);
        include(road.to, road.halfWidth);
    }
    for (const auto& landmark : layout.Landmarks())
    {
        include(landmark.position, 90.f);
    }
    const float mapWidth = (std::max)(1.f, maxX - minX);
    const float mapHeight = (std::max)(1.f, maxY - minY);
    const float legendWidth = 242.f;
    const float scale =
        (std::min)((panelWidth - legendWidth - 54.f) / mapWidth, (panelHeight - 100.f) / mapHeight);
    const float mapX = x + legendWidth + 22.f;
    const float mapY = y + 56.f;
    const auto project = [&](WorldPosition world)
    {
        return Point{mapX + (world.x - minX) * scale, mapY + (world.y - minY) * scale};
    };
    for (const auto& road : roads)
    {
        const Point a = project(road.from);
        const Point b = project(road.to);
        const float dx = b.x - a.x;
        const float dy = b.y - a.y;
        const float length = std::sqrt(dx * dx + dy * dy);
        if (length <= 0.f)
        {
            continue;
        }
        const float nx = -dy / length * 1.5f;
        const float ny = dx / length * 1.5f;
        r.Quad({a.x + nx, a.y + ny},
               {b.x + nx, b.y + ny},
               {b.x - nx, b.y - ny},
               {a.x - nx, a.y - ny},
               Color(.52f, .48f, .31f));
    }
    for (std::size_t i = 0; i < areas.size(); ++i)
    {
        const Point p = project(areas[i].center);
        r.Ellipse(p.x, p.y, 6, 6, Color(.44f, .65f, .57f));
        r.Text(p.x + 9, p.y + 4, std::to_string(i + 1), Color(.87f, .88f, .75f));
        r.Text(x + 18,
               y + 53 + float(i) * 22,
               std::to_string(i + 1) + ". " + areas[i].name,
               Color(.69f, .78f, .73f));
    }
    const Point p = project(player);
    r.Ellipse(p.x, p.y, 4, 4, Color(1.f, .78f, .20f));
    r.Text(x + 18,
           y + panelHeight - 19,
           "황금 점: 플레이어 · 지도 경계 밖은 이동할 수 없습니다.",
           Color(.75f, .72f, .55f));
}

void MapView::ToggleOverview()
{
    m_OverviewOpen = !m_OverviewOpen;
}

void MapView::CloseOverview()
{
    m_OverviewOpen = false;
}
