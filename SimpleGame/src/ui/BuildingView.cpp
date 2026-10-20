#include "stdafx.h"
#include "BuildingView.h"
#include "SceneCamera.h"

void BuildingView::Draw(Renderer& r, const MapBuilding& building, Point p)
{
    const auto& bounds = building.footprint;
    const float w = bounds.halfWidth * 2.f;
    const float d = bounds.halfHeight * 2.f * SceneCamera::GroundDepthScale();
    const float h = building.height * .5f;
    const float x = p.x - bounds.halfWidth;
    const bool archive = building.style == BuildingStyle::Archive;
    const bool wall = building.style == BuildingStyle::Wall;
    const bool shelf = building.style == BuildingStyle::Bookshelf;
    const Color stone(.43f, .45f, .43f);
    const Color edge(.17f, .21f, .23f);
    const Color light(.67f, .64f, .51f);
    const Color roof = archive                                      ? Color(.20f, .30f, .34f)
                       : building.style == BuildingStyle::Monastery ? Color(.27f, .29f, .31f)
                       : shelf                                      ? Color(.32f, .23f, .15f)
                                                                    : Color(.40f, .23f, .19f);
    r.Rect(x + 14, p.y - d + 12, w, d + 10, Color(0, 0, 0, .25f));
    r.Rect(x, p.y - d - h, w, d, wall ? stone : roof);
    r.Rect(x, p.y - h, w, h, wall || archive ? stone : Color(.49f, .43f, .33f));
    r.Rect(x, p.y - d - h, w, 5, light);
    r.Rect(x, p.y - h - 5, w, 7, edge);
    r.Rect(x, p.y - 4, w, 4, edge);
    if (wall)
    {
        for (float offset = 0; offset < w; offset += 42.f)
        {
            r.Rect(x + offset, p.y - d - h - 8, 20, 12, light);
        }
        for (float offset = 0; offset < h; offset += 18.f)
        {
            r.Rect(x, p.y - h + offset, w, 2, edge);
        }
        return;
    }
    if (shelf)
    {
        for (int row = 0; row < 3; ++row)
        {
            for (int book = 0; book < 10; ++book)
            {
                r.Rect(x + 7 + book * 14.f,
                       p.y - h + 5 + row * 18.f,
                       9,
                       13,
                       (book + row) % 2 ? light : Color(.25f, .38f, .33f));
            }
        }
        return;
    }
    for (float offset = 35.f; offset < w - 25.f; offset += 80.f)
    {
        r.Rect(x + offset, p.y - h + 20, 24, 28, edge);
        r.Rect(x + offset + 4, p.y - h + 24, 16, 18, Color(.75f, .66f, .39f));
        if (archive)
        {
            r.Rect(x + offset - 9, p.y - h + 8, 8, h - 12, light);
        }
    }
    r.Rect(p.x - 24, p.y - 48, 48, 44, edge);
    if (archive)
    {
        r.Rect(p.x - 30, p.y - 54, 60, 6, light);
        r.Rect(p.x - 15, p.y - 40, 30, 32, Color(.22f, .47f, .53f));
        r.Rect(p.x - 4, p.y - 36, 8, 26, light);
    }
    else
    {
        // Facade only: closed doors have no portal or entry trigger.
        r.Rect(p.x - 26, p.y - 31, 52, 6, Color(.47f, .30f, .19f));
    }
    r.Rect(p.x - 115, p.y - h - 25, 230, 21, Color(.035f, .055f, .065f, .9f));
    r.Text(p.x - r.TextWidth(building.name) * .5f, p.y - h - 9, building.name, light);
}
