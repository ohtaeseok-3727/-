#include "stdafx.h"
#include "LevelOneView.h"
#include <algorithm>
#include <cmath>
#include <sstream>

void LevelOneView::DrawTile(Renderer& r, const LevelMap& map, int x, int y, Point p) const
{
    const float worldX = x * 64.f + 32.f;
    const float worldY = y * 64.f + 32.f;
    const float tileHeight = 64.f * .8660254f;
    const float shade = float(map.Hash(x, y) % 19) / 1000.f;
    const bool road = map.IsRoad(worldX, worldY);
    const bool town = map.IsTown(worldX, worldY);
    const bool farmland = worldX > 230.f && worldX < 1500.f && std::abs(worldY) < 600.f;
    const bool river = worldY >= 768.f && worldY < 896.f;
    Color color(.14f + shade, .19f + shade, .17f + shade);
    if (worldX > 1500.f)
    {
        color = Color(.18f + shade, .17f + shade, .15f + shade);
    }
    if (farmland)
    {
        color = Color(.24f + shade, .23f + shade, .15f + shade);
    }
    if (river)
    {
        color = Color(.12f + shade, .27f + shade, .31f + shade);
    }
    if (road)
    {
        color = town ? Color(.29f + shade, .28f + shade, .24f + shade)
                     : Color(.27f + shade, .24f + shade, .18f + shade);
    }
    r.Rect(p.x, p.y, 64.5f, tileHeight + .5f, color);
    if (farmland && !road)
    {
        for (int row = 0; row < 4; ++row)
        {
            r.Rect(p.x + 4, p.y + row * 13.f + 4, 55, 3, Color(.15f, .18f, .10f));
            r.Rect(p.x + 9, p.y + row * 13.f + 3, 6, 4, Color(.35f, .39f, .16f));
        }
    }
    else if (river && road)
    {
        for (int row = 0; row < 5; ++row)
        {
            r.Rect(p.x + 2, p.y + row * 11.f, 60, 3, Color(.40f, .33f, .22f));
        }
    }
    else if (river)
    {
        r.Rect(p.x + 10, p.y + 20, 24, 2, Color(.24f, .43f, .44f));
    }
    else if (!road)
    {
        const float gx = p.x + float(map.Hash(x, y, 19) % 53);
        r.Triangle({gx, p.y + 20}, {gx + 3, p.y + 15}, {gx + 5, p.y + 20}, Color(.25f, .29f, .22f));
    }
}

void LevelOneView::DrawEnemy(Renderer& r, const FieldEnemy& enemy, Point p) const
{
    // Temporary geometric creature; no new sprite sheet or character art is substituted.
    Color body = enemy.hitFlash > 0.f ? Color(.95f, .89f, .66f)
                 : enemy.windup > 0.f ? Color(.95f, .39f, .12f)
                                      : Color(.47f, .58f, .29f);
    r.Ellipse(p.x, p.y, 16, 7, Color(.02f, .03f, .02f, .5f));
    r.Ellipse(p.x, p.y - 15, 17, 15, body);
    r.Rect(p.x - 7, p.y - 20, 4, 5, Color(.07f, .09f, .05f));
    r.Rect(p.x + 4, p.y - 20, 4, 5, Color(.07f, .09f, .05f));
    r.Rect(p.x - 18, p.y - 39, 36, 4, Color(.14f, .10f, .10f));
    r.Rect(p.x - 18,
           p.y - 39,
           36.f * enemy.health / LevelTuning::EnemyHealth,
           4,
           Color(.70f, .26f, .19f));
    if (enemy.windup > 0.f)
    {
        r.Text(p.x - 4, p.y - 48, "!", Color(1.f, .66f, .25f));
    }
}

void LevelOneView::DrawVillageHouse(Renderer& r, Point p) const
{
    r.Rect(p.x - 28, p.y - 54, 56, 64, Color(.36f, .32f, .25f));
    r.Triangle(
        {p.x - 36, p.y - 50}, {p.x, p.y - 85}, {p.x + 36, p.y - 50}, Color(.32f, .17f, .14f));
    r.Rect(p.x - 8, p.y - 15, 16, 25, Color(.14f, .12f, .10f));
    r.Rect(p.x - 21, p.y - 38, 11, 13, Color(.88f, .65f, .31f));
    r.Rect(p.x + 10, p.y - 38, 11, 13, Color(.88f, .65f, .31f));
}

void LevelOneView::DrawDrop(Renderer& r, const LootDrop& drop, Point p) const
{
    const float bob = std::sin(drop.age * 6.f) * 2.f;
    r.Ellipse(p.x, p.y, 7, 3, Color(.02f, .03f, .02f, .4f));
    if (drop.kind == LootKind::Gold)
    {
        r.Ellipse(p.x, p.y - 8 + bob, 5, 6, Color(1.f, .77f, .20f));
    }
    else if (drop.kind == LootKind::Health)
    {
        r.Rect(p.x - 6, p.y - 12 + bob, 12, 10, Color(.85f, .18f, .20f));
        r.Rect(p.x - 1, p.y - 11 + bob, 2, 8, Color(1.f, .88f, .84f));
        r.Rect(p.x - 4, p.y - 8 + bob, 8, 2, Color(1.f, .88f, .84f));
    }
    else
    {
        r.Quad({p.x, p.y - 17 + bob},
               {p.x + 7, p.y - 9 + bob},
               {p.x, p.y - 1 + bob},
               {p.x - 7, p.y - 9 + bob},
               Color(.30f, .70f, 1.f));
    }
}

void LevelOneView::DrawHud(
    Renderer& r, const LevelOneSession& session, const char* region, int width, int height) const
{
    const auto& progress = session.Progress();
    const auto& inventory = session.Inventory();
    r.Rect(0, 0, float(width), 116, Color(.025f, .04f, .045f, .96f));
    r.Text(20, 23, std::string("LEVEL 1 / ") + region, Color(.87f, .79f, .57f));
    std::ostringstream stats;
    stats << "LV " << progress.Level() << "   HP " << progress.Health() << "/"
          << progress.MaxHealth() << "   ATK " << progress.Attack() + inventory.AttackBonus()
          << "   GOLD " << inventory.Gold() << "   GEAR " << inventory.Items().size();
    r.Text(20, 44, stats.str(), Color(.83f, .85f, .78f));
    r.Rect(20, 54, 200, 8, Color(.18f, .09f, .08f));
    r.Rect(20, 54, 200.f * progress.Health() / progress.MaxHealth(), 8, Color(.72f, .25f, .19f));
    r.Rect(240, 54, 200, 8, Color(.08f, .15f, .18f));
    r.Rect(240,
           54,
           200.f * progress.Experience() / progress.RequiredExperience(),
           8,
           Color(.29f, .69f, .80f));
    r.Text(455,
           62,
           "EXP " + std::to_string(progress.Experience()) + " / " +
               std::to_string(progress.RequiredExperience()),
           Color(.65f, .83f, .86f));
    r.Text(20, 85, session.Objective(), Color(.84f, .80f, .64f));
    r.Text(20,
           105,
           "WASD / ARROWS MOVE   SHIFT RUN   CTRL COMBO   I INVENTORY   P PAUSE   R VILLAGE",
           Color(.57f, .65f, .61f));
    r.Rect(0, float(height - 42), float(width), 42, Color(.025f, .04f, .045f, .95f));
    r.Text(20, float(height - 17), session.Notice(), Color(.95f, .84f, .57f));
    if (!m_InventoryOpen)
    {
        return;
    }
    const float x = (std::max)(12.f, width * .5f - 260.f);
    const float y = 132.f;
    r.Rect(x, y, 520, 315, Color(.035f, .055f, .065f, .98f));
    r.Text(x + 20, y + 25, "INVENTORY [I]   PAGE UP / DOWN", Color(.88f, .81f, .64f));
    r.Text(x + 20,
           y + 49,
           "Auto-equip best gear. Collected gear stays in your bag.",
           Color(.57f, .70f, .75f));
    const int count = static_cast<int>(inventory.Items().size());
    const int pages = (std::max)(1, (count + 7) / 8);
    const int page = (std::min)(m_Page, pages - 1);
    r.Text(x + 20,
           y + 72,
           "Gold: " + std::to_string(inventory.Gold()) + "   Page " + std::to_string(page + 1) +
               " / " + std::to_string(pages),
           Color(.9f, .74f, .35f));
    for (int i = page * 8; i < (std::min)(count, page * 8 + 8); ++i)
    {
        const auto& item = inventory.Items()[i];
        const std::string label = (i == inventory.EquippedIndex() ? "[E] " : "    ") + item.name +
                                  "  ATK +" + std::to_string(item.attack);
        r.Text(x + 20, y + 100 + (i - page * 8) * 22.f, label, Color(.69f, .80f, .87f));
    }
    r.Text(x + 20,
           y + 294,
           "STR " + std::to_string(progress.Strength()) + "   INT " +
               std::to_string(progress.Intelligence()) + "   MAX MP " +
               std::to_string(progress.MaxMana()),
           Color(.64f, .72f, .67f));
}

void LevelOneView::ToggleInventory()
{
    m_InventoryOpen = !m_InventoryOpen;
}

void LevelOneView::ChangePage(int direction, int itemCount)
{
    if (m_InventoryOpen)
    {
        const int lastPage = (std::max)(0, (itemCount - 1) / 8);
        m_Page = (std::max)(0, (std::min)(lastPage, m_Page + direction));
    }
}
