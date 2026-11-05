#pragma once

#include "PlayerInventory.h"
#include "Renderer.h"

// Prototype prices and stock, pending game design confirmation.
class TradeSystem
{
  public:
    static constexpr int HealthPrice = 20;
    static constexpr int ManaPrice = 20;
    static constexpr int EquipmentPrice = 60;
    static constexpr int EquipmentAttack = 5;
    static constexpr int HealthRecovery = 50;
    static constexpr int ManaRecovery = 30;
    void Toggle();
    void Close();
    bool IsOpen() const;
    void Draw(Renderer& renderer, const PlayerInventory& inventory, int width, int height) const;
    void MouseButton(
        bool down, int x, int y, int width, int height, int profession, PlayerInventory& inventory);
    void CancelPress();

  private:
    int Hit(int x, int y, int width, int height, int itemCount) const;
    bool m_Open = false;
    int m_Pressed = -1;
    int m_Page = 0;
    std::string m_Message;
};
