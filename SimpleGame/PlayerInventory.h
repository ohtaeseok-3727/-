#pragma once

#include <vector>
#include <string>

struct Equipment
{
    std::string name;
    int attack;
};

class PlayerInventory
{
  public:
    void Reset();
    void AddGold(int amount);
    void AddEquipment(int profession, int quality);
    int Gold() const;
    int AttackBonus() const;
    int EquippedIndex() const;
    const std::vector<Equipment>& Items() const;

  private:
    int m_Gold = 0;
    int m_Equipped = -1;
    std::vector<Equipment> m_Items;
};
