#include "stdafx.h"
#include "PrototypeWorld.h"
#include <algorithm>
#include <cmath>
#include <random>

namespace
{
    const float ChunkSize = 512.f;
    // Orthographic view: angle between the sight line and the ground plane.
    const float GroundViewAngle = 60.f * 3.14159265358979323846f / 180.f;
    const float GroundDepthScale = std::sin(GroundViewAngle);
    const float ObjectHeightScale = std::cos(GroundViewAngle);
}

void PrototypeWorld::Reset()
{
    if (!m_MapGenerated)
    {
        m_Map.Generate(std::random_device{}());
        m_MapGenerated = true;
    }

    m_X = m_Y = m_CameraX = m_CameraY = m_Time = m_Walk = 0;
    m_FaceLeft = false;
    m_Moving = false;
    m_Attack.Reset();
    m_Chunks.clear();
    Stream();
}

void PrototypeWorld::ToggleInventory()
{
    if (m_Started)
    {
        m_LevelView.ToggleInventory();
    }
}

void PrototypeWorld::ChangeInventoryPage(int direction)
{
    m_LevelView.ChangePage(direction, static_cast<int>(m_Level.Inventory().Items().size()));
}

void PrototypeWorld::ToggleMap()
{
    if (m_Started)
    {
        m_MapView.ToggleOverview();
    }
}

void PrototypeWorld::InspectLandmark()
{
    if (!m_Started || m_Paused)
    {
        return;
    }
    const auto* site = m_Map.Layout().NearbyLandmark({m_X, m_Y});
    if (site)
    {
        m_Level.Inspect(*site);
    }
}

void PrototypeWorld::Stream()
{
    int cx = static_cast<int>(std::floor(m_X / ChunkSize)),
        cy = static_cast<int>(std::floor(m_Y / ChunkSize));
    // Include tall props and a prefetch margin even after the window is resized.
    int radiusX = static_cast<int>(std::ceil(m_Width * .5f / ChunkSize)) + 2;
    int radiusY = static_cast<int>(std::ceil(m_Height * .6f / GroundDepthScale / ChunkSize)) + 2;
    for (auto it = m_Chunks.begin(); it != m_Chunks.end();)
    {
        if (std::abs(it->first.first - cx) > radiusX || std::abs(it->first.second - cy) > radiusY)
            it = m_Chunks.erase(it);
        else
            ++it;
    }
    for (int y = cy - radiusY; y <= cy + radiusY; ++y)
        for (int x = cx - radiusX; x <= cx + radiusX; ++x)
        {
            auto key = std::make_pair(x, y);
            if (m_Chunks.count(key))
                continue;
            Chunk chunk;
            chunk.objects = m_Map.ChunkObjects(x, y);
            m_Chunks.emplace(key, chunk);
        }
}

bool PrototypeWorld::Blocked(float x, float y) const
{
    return m_Map.Blocked(x, y);
}

void PrototypeWorld::Update(float dt, float dx, float dy, bool sprint, bool attackPressed)
{
    if (m_Chunks.empty())
        Stream();
    if (!m_Started || m_Paused)
        return;

    m_Time += dt;

    m_Attack.Update(dt);

    for (const int step : m_Attack.ConsumeHits())
    {
        m_Level.Strike(step, {m_X, m_Y}, m_FaceLeft, m_Map);
    }

    if (attackPressed)
    {
        // Pick the initial facing from movement, then hold it through the strike.
        if (!m_Attack.IsAttacking() && std::abs(dx) > 0.0001f)
        {
            m_FaceLeft = dx < 0.f;
        }

        m_Attack.Press(m_Class);
    }

    // Implementation assumption: each strike keeps the player in place.
    if (m_Attack.IsAttacking())
    {
        dx = 0.f;
        dy = 0.f;
    }

    m_Moving = false;
    float length = std::sqrt(dx * dx + dy * dy);
    if (length > 0)
    {
        float oldX = m_X, oldY = m_Y;
        dx /= length;
        dy /= length;
        float distance = (sprint ? 240.f : 145.f) * dt;
        // Short substeps prevent tunnelling through small props on slow frames.
        int steps = static_cast<int>(std::ceil(distance / 4.f));
        for (int i = 0; i < steps; ++i)
        {
            float nx = m_X + dx * distance / steps, ny = m_Y + dy * distance / steps;
            if (!Blocked(nx, m_Y))
                m_X = nx;
            if (!Blocked(m_X, ny))
                m_Y = ny;
        }
        float movedX = m_X - oldX, movedY = m_Y - oldY;
        float traveled = std::sqrt(movedX * movedX + movedY * movedY);
        if (traveled > 0.0001f)
        {
            m_Moving = true;
            if (std::abs(movedX) > 0.0001f)
                m_FaceLeft = movedX < 0;
            // Preserve the 80-world-unit stride with all eight mini walk frames.
            m_Walk = std::fmod(m_Walk + traveled / 10.f, 8.f);
        }
        else
            m_Walk = 0;
    }
    else
        m_Walk = 0;
    m_Level.Update(dt, {m_X, m_Y}, m_Map);
    if (m_Level.IsDefeated())
    {
        Reset();
        m_Level.Revive();
    }

    Stream();
    float follow = 1.f - std::exp(-9.f * dt);
    m_CameraX += (m_X - m_CameraX) * follow;
    m_CameraY += (m_Y - m_CameraY) * follow;
}

Point PrototypeWorld::Project(float x, float y) const
{
    return {m_Width * .5f + (x - m_CameraX), m_Height * .52f + (y - m_CameraY) * GroundDepthScale};
}

void PrototypeWorld::DrawObject(Renderer& r, const Object& o)
{
    Point p = Project(o.x, o.y);
    float x = p.x, y = p.y, s = o.size;
    // Solid props share the ground projection; characters/trees remain upright billboards.
    const auto point = [&](float dx, float depth, float height)
    {
        Point q = Project(o.x + dx * s, o.y + depth * s);
        q.y -= height * ObjectHeightScale * s;
        return q;
    };
    if (x < -180 || x > m_Width + 180 || y < -80 || y > m_Height + 230)
        return;
    if (o.kind == 9)
    {
        m_MapView.DrawLandmark(r, m_Map.Layout().Landmarks()[o.entity], p);
    }
    else if (o.kind == 6)
    {
        m_LevelView.DrawEnemy(r, m_Level.Enemies().Enemies()[o.entity], p);
    }
    else if (o.kind == 7)
    {
        m_LevelView.DrawDrop(r, m_Level.Loot().Drops()[o.entity], p);
    }
    else if (o.kind == 5)
    {
        if (m_Attack.IsAttacking() &&
            r.CharacterAttack(
                m_Class, m_Attack.ClipId(), m_Attack.ElapsedMilliseconds(), x, y, m_FaceLeft))
        {
            return;
        }

        int frame = m_Moving ? static_cast<int>(m_Walk) : 0;
        r.Character(m_Class, x, y, m_FaceLeft, 1, -1, frame);
    }
    else if (o.kind == 0)
    {
        r.Quad({x - 7 * s, y},
               {x + 7 * s, y},
               {x + 3 * s, y - 104 * s},
               {x - 4 * s, y - 108 * s},
               Color(.19f, .18f, .16f));
        r.Quad({x, y - 52 * s},
               {x - 38 * s, y - 83 * s},
               {x - 42 * s, y - 106 * s},
               {x - 30 * s, y - 83 * s},
               Color(.22f, .21f, .18f));
        r.Quad({x, y - 73 * s},
               {x + 34 * s, y - 101 * s},
               {x + 39 * s, y - 129 * s},
               {x + 27 * s, y - 102 * s},
               Color(.23f, .22f, .18f));
        r.Triangle({x - 4 * s, y - 89 * s},
                   {x + 3 * s, y - 126 * s},
                   {x + 7 * s, y - 74 * s},
                   Color(.26f, .25f, .21f));
    }
    else if (o.kind == 1)
    {
        r.Quad(point(-38, 14, 0),
               point(25, 14, 0),
               point(25, 14, 128),
               point(-38, 14, 104),
               Color(.36f, .38f, .35f));
        r.Quad(point(25, 14, 0),
               point(40, -28, 0),
               point(40, -28, 128),
               point(25, 14, 128),
               Color(.23f, .27f, .27f));
        r.Quad(point(-38, 14, 104),
               point(25, 14, 128),
               point(40, -28, 128),
               point(-22, -28, 104),
               Color(.48f, .49f, .42f));
        for (int i = 1; i < 4; ++i)
        {
            Point seam = point(-37, 14, float(i * 26));
            r.Rect(seam.x, seam.y, 61 * s, 2 * s, Color(.23f, .26f, .25f));
        }
        Point crack = point(-7, 14, 96);
        r.Rect(crack.x, crack.y, 3 * s, 19 * s, Color(.19f, .23f, .22f));
        crack = point(10, 14, 42);
        r.Rect(crack.x, crack.y, 3 * s, 19 * s, Color(.19f, .23f, .22f));
        r.Triangle(
            point(-32, 18, 0), point(-49, 28, 0), point(-18, 30, 0), Color(.38f, .39f, .34f));
    }
    else if (o.kind == 2)
    {
        r.Quad(point(-20, 10, 0),
               point(-13, -10, 28),
               point(11, -14, 32),
               point(22, 7, 0),
               Color(.30f, .34f, .32f));
        r.Triangle(
            point(-13, -10, 28), point(11, -14, 32), point(2, 5, 24), Color(.43f, .46f, .40f));
    }
    else if (o.kind == 3)
    {
        r.Rect(x - 2, y - 96, 4, 96, Color(.39f, .33f, .24f));
        float sway = std::sin(m_Time * 2) * 4;
        r.Quad({x + 2, y - 94},
               {x + 43, y - 90 + sway},
               {x + 36, y - 51 + sway},
               {x + 2, y - 60},
               Color(.43f, .19f, .17f));
        r.Rect(x + 14, y - 81, 3, 19, Color(.70f, .57f, .36f));
    }
    else
    {
        r.Rect(x - 14, y - 5, 28, 5, Color(.22f, .18f, .13f));
        float flicker = std::sin(m_Time * 9) * 4;
        // Draw only the emitter. The post-process bright pass supplies all light bloom.
        // Gentle intensity variation changes the bloom without drawing a flat halo.
        float intensity = 1.f + .06f * std::sin(m_Time * 7.f);
        r.Triangle({x - 10, y - 5},
                   {x + 11, y - 5},
                   {x + flicker, y - 37},
                   Color(1.65f * intensity, .65f * intensity, .16f * intensity));
        r.Triangle({x - 5, y - 6},
                   {x + 5, y - 6},
                   {x - flicker, y - 25},
                   Color(2.4f * intensity, 1.25f * intensity, .38f * intensity));
    }
}

void PrototypeWorld::Draw(Renderer& r, int width, int height)
{
    m_Width = width;
    m_Height = height;
    r.Begin();
    int left = int(std::floor((m_CameraX - width * .5f) / 64.f)) - 1;
    int top = int(std::floor((m_CameraY - height / GroundDepthScale * .52f) / 64.f)) - 1;
    int cols = width / 64 + 4, rows = int(height / (64 * GroundDepthScale)) + 5;
    for (int y = top; y < top + rows; ++y)
        for (int x = left; x < left + cols; ++x)
        {
            Point p = Project(x * 64.f, y * 64.f);
            m_MapView.DrawTile(r, m_Map, x, y, p);
        }
    std::vector<Object> objects;
    for (const auto& entry : m_Chunks)
        for (const Object& o : entry.second.objects)
        {
            objects.push_back(o);
            Point p = Project(o.x, o.y);
            if (p.x > -150 && p.x < width + 150 && p.y > -80 && p.y < height + 80)
                r.Ellipse(p.x + 8,
                          p.y,
                          30 * o.size,
                          19 * GroundDepthScale * o.size,
                          Color(.025f, .035f, .035f, .35f));
        }
    if (m_Started)
    {
        const auto& enemies = m_Level.Enemies().Enemies();
        for (std::size_t i = 0; i < enemies.size(); ++i)
        {
            if (enemies[i].health > 0)
            {
                objects.push_back(
                    {enemies[i].position.x, enemies[i].position.y, 6, 1.f, static_cast<int>(i)});
            }
        }
        const auto& drops = m_Level.Loot().Drops();
        for (std::size_t i = 0; i < drops.size(); ++i)
        {
            objects.push_back(
                {drops[i].position.x, drops[i].position.y, 7, 1.f, static_cast<int>(i)});
        }

        objects.push_back({m_X, m_Y, 5, 1});
        Point p = Project(m_X, m_Y);
        r.Ellipse(p.x, p.y, 10, 7 * GroundDepthScale, Color(.015f, .025f, .03f, .6f));
        r.Ellipse(p.x, p.y, 11, 7 * GroundDepthScale, Color(.7f, .62f, .37f, .13f));
    }
    std::stable_sort(objects.begin(),
                     objects.end(),
                     [](const Object& a, const Object& b)
                     {
                         return a.y < b.y;
                     });
    for (const Object& o : objects)
        DrawObject(r, o);
    r.FinishWorld(); // Post-process only the completed world; UI stays sharp and ungraded.
    if (m_Started)
    {
        m_LevelView.DrawHud(r, m_Level, m_Map.RegionName(m_X, m_Y), width, height);
        const auto* site = m_Map.Layout().NearbyLandmark({m_X, m_Y});
        if (site)
        {
            r.Text(20, 134, std::string("[E] ") + site->name, Color(.94f, .83f, .51f));
        }
        m_MapView.DrawOverview(r, m_Map, {m_X, m_Y}, width, height);
        if (m_Paused)
        {
            r.Text(width * .5f - 50, height * .5f, "PAUSED [P]", Color(1.f, .85f, .56f));
        }
        r.Flush();
        return;
    }
    // Quiet letterbox and readable UI, independent of world coordinates.
    r.Rect(0, 0, float(width), 78, Color(.025f, .04f, .045f, .94f));
    r.Rect(0, 78, float(width), 1, Color(.48f, .42f, .27f, .65f));
    r.Rect(0, float(height - 46), float(width), 46, Color(.025f, .04f, .045f, .94f));
    r.Text(24, 28, "THE LONG WAR  /  MERCENARY PROTOTYPE", Color(.82f, .76f, .60f));
    r.Text(24, 53, "A country worn down by centuries of war.", Color(.52f, .60f, .59f));
    r.Text(24, 71, r.EffectStatus(), Color(.58f, .63f, .61f));
    if (!m_Started)
    {
        float x = width * .5f - 250, y = height * .5f - 165;
        r.Rect(x, y, 500, 330, Color(.04f, .06f, .065f, .97f));
        r.Rect(x, y, 500, 2, Color(.65f, .54f, .33f));
        r.Text(x + 28, y + 39, "CHOOSE YOUR MERCENARY", Color(.91f, .83f, .65f));
        r.Character(0, x + 135, y + 180);
        r.Character(1, x + 365, y + 180);
        r.Text(x + 95, y + 213, "[1] MAGE", Color(.76f, .79f, .75f));
        r.Text(x + 315, y + 213, "[2] KNIGHT", Color(.76f, .79f, .75f));
        r.Text(x + 65, y + 240, "RUNIC GLOVES", Color(.42f, .76f, .80f));
        r.Text(x + 305, y + 240, "SWORD & SHIELD", Color(.69f, .72f, .69f));
        r.Text(x + 28, y + 278, "Press 1 or 2 to enter the field.", Color(.57f, .64f, .61f));
        r.Text(x + 28, y + 309, "Tap CTRL to chain attacks: 1 > 2 > 3.", Color(.48f, .54f, .53f));
    }
    r.Text(float(width - 470),
           52,
           "WASD / ARROWS   SHIFT RUN   P PAUSE   R HOME",
           Color(.64f, .68f, .63f));
    r.Text(float(width - 470), 71, "CTRL ATTACK   TAP FOR 1 > 2 > 3", Color(.64f, .68f, .63f));
    r.Flush();
}
