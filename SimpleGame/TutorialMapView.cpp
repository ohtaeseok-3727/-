#include "stdafx.h"
#include "TutorialMapView.h"
#include <algorithm>
#include <cmath>
#include <string>

void TutorialMapView::DrawTile(Renderer& r, const LevelMap& map, int x, int y, Point p) const
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
            const bool mine = layout.InMineBounds(world);
            const bool water = layout.River(world);
            const bool road = map.IsRoad(world.x, world.y);
            Color color(.13f + shade, .18f + shade, .15f + shade);
            if (region == TutorialRegion::Farmland)
            {
                color = Color(.26f + shade, .25f + shade, .16f + shade);
            }
            else if (region == TutorialRegion::Woods || region == TutorialRegion::Outpost)
            {
                color = Color(.10f + shade, .15f + shade, .12f + shade);
            }
            else if (region == TutorialRegion::Battlefield)
            {
                color = Color(.20f + shade, .18f + shade, .16f + shade);
            }
            else if (region == TutorialRegion::MineEntrance)
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
                color = layout.MineFloor(world) ? Color(.24f + shade, .23f + shade, .21f + shade)
                                                : Color(.045f, .055f, .065f);
            }
            const float px = p.x + col * 16.f;
            const float py = p.y + row * 16.f * .8660254f;
            r.Rect(px, py, 16.5f, 14.4f, color);
            if (!mine && region == TutorialRegion::Farmland && !road)
            {
                r.Rect(px + 2, py + 4, 12, 2, Color(.16f, .19f, .10f));
                r.Rect(px + 4, py + 3, 3, 3, Color(.43f, .44f, .19f));
            }
            if (water && road)
            {
                r.Rect(px, py, 16, 2, Color(.47f, .36f, .23f));
            }
            if (!mine && region == TutorialRegion::Battlefield && !road &&
                map.Hash(x * 4 + col, y * 4 + row, 5) % 13 == 0)
            {
                r.Rect(px + 2, py + 3, 12, 5, Color(.10f, .11f, .10f));
            }
        }
    }
}

void TutorialMapView::DrawLandmark(Renderer& r, const TutorialLandmark& site, Point p) const
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
        r.Ellipse(p.x, p.y - 35, 9, 10, Color(.63f, .51f, .38f));
        r.Rect(p.x - 9, p.y - 26, 18, 24, Color(.32f, .37f, .35f));
        r.Rect(p.x - 10, p.y - 2, 7, 7, wood);
        r.Rect(p.x + 3, p.y - 2, 7, 7, wood);
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
    const float labelWidth = float(std::char_traits<char>::length(site.name)) * 8.f;
    r.Text(p.x - labelWidth * .5f, p.y - 99, site.name, Color(.83f, .80f, .64f));
}

void TutorialMapView::DrawOverview(
    Renderer& r, const LevelMap& map, WorldPosition player, int width, int height) const
{
    if (!m_OverviewOpen)
    {
        return;
    }
    const float panelWidth = (std::min)(float(width - 32), 1080.f);
    const float panelHeight = (std::min)(float(height - 150), 560.f);
    if (panelWidth < 520 || panelHeight < 350)
    {
        r.Text(20, 150, "Enlarge the window to view the route map. [M]", Color(1.f, .8f, .5f));
        return;
    }
    const float x = (width - panelWidth) * .5f;
    const float y = 128.f;
    r.Rect(x, y, panelWidth, panelHeight, Color(.025f, .045f, .052f, .98f));
    r.Text(
        x + 18, y + 25, "PROLOGUE ROUTES [M]   E: READ NEARBY LANDMARK", Color(.91f, .83f, .64f));
    const float scale = (std::min)((panelWidth - 290.f) / 6600.f, (panelHeight - 95.f) / 6200.f);
    const auto project = [&](WorldPosition world)
    {
        return Point{x + 260.f + (world.x + 1850.f) * scale, y + 50.f + (world.y + 5850.f) * scale};
    };
    for (const auto& road : map.Layout().Roads())
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
    const auto& areas = map.Layout().Areas();
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
    if (player.x >= -1850.f && player.x <= 4750.f && player.y >= -5850.f && player.y <= 350.f)
    {
        const Point p = project(player);
        r.Ellipse(p.x, p.y, 4, 4, Color(1.f, .78f, .20f));
    }
    r.Text(x + 18,
           y + panelHeight - 19,
           "Gold dot: you. Three investigations are optional-order; mine branches reconnect.",
           Color(.75f, .72f, .55f));
}

void TutorialMapView::ToggleOverview()
{
    m_OverviewOpen = !m_OverviewOpen;
}
