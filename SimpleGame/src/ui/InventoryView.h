#pragma once

#include "PlayerInventory.h"
#include "Renderer.h"

// Draws the inventory's visual layout while PlayerInventory remains the data owner.
class InventoryView
{
  public:
    void Draw(Renderer& renderer,
              const PlayerInventory& inventory,
              int viewportWidth,
              int viewportHeight) const;
    void Toggle();
    void Close();
    void ChangePage(int direction, int itemCount);

  private:
    void DrawEquipmentIcon(Renderer& renderer,
                           const Equipment& equipment,
                           float centerX,
                           float centerY) const;
    bool m_IsOpen = false;
    int m_Page = 0;
};
