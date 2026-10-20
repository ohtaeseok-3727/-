#pragma once

#include "InventoryView.h"
#include "Renderer.h"
#include "GameSession.h"

class GameView
{
  public:
    void DrawDrop(Renderer& renderer, const LootDrop& drop, Point screen) const;
    void DrawHud(Renderer& renderer,
                 const GameSession& session,
                 const char* region,
                 int width,
                 int height) const;
    void ToggleInventory();
    void CloseInventory();

    bool InventoryOpen() const
    {
        return m_InventoryView.IsOpen();
    }

    void ChangePage(int direction, int itemCount);

  private:
    InventoryView m_InventoryView;
};
