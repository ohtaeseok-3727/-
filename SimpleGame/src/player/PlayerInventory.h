#pragma once

#include <string>
#include <vector>

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
    bool SpendGold(int amount);
    void AddEquipment(int profession, int quality);
    bool Equip(int index);
    bool SellEquipment(int index, int price);
    bool BuyPotion(bool mana, int price);
    bool UsePotion(bool mana);
    int HealthPotions() const;
    int ManaPotions() const;
    int Gold() const;
    int AttackBonus() const;
    int EquippedIndex() const;
    const std::vector<Equipment>& Items() const;

  private:
    friend class SaveArchive;
    int m_Gold = 0;
    int m_Equipped = -1;
    std::vector<Equipment> m_Items;
    int m_HealthPotions = 0;
    int m_ManaPotions = 0;
};
