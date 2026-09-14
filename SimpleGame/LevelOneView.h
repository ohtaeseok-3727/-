#pragma once

#include "Renderer.h"
#include "LevelOneSession.h"

class LevelOneView
{
  public:
    void DrawTile(Renderer& renderer, const LevelMap& map, int x, int y, Point screen) const;
    void DrawEnemy(Renderer& renderer, const FieldEnemy& enemy, Point screen) const;
    void DrawDrop(Renderer& renderer, const LootDrop& drop, Point screen) const;
    void DrawVillageHouse(Renderer& renderer, Point screen) const;
    void DrawHud(Renderer& renderer,
                 const LevelOneSession& session,
                 const char* region,
                 int width,
                 int height) const;
    void ToggleInventory();
    void ChangePage(int direction, int itemCount);

  private:
    bool m_InventoryOpen = false;
    int m_Page = 0;
};
