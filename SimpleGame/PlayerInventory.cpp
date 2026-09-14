#include "stdafx.h"
#include "PlayerInventory.h"

void PlayerInventory::Reset()
{
    m_Gold = 0;
    m_Equipped = -1;
    m_Items.clear();
}

void PlayerInventory::AddGold(int amount)
{
    if (amount > 0)
    {
        m_Gold += amount;
    }
}

void PlayerInventory::AddEquipment(int profession, int quality)
{
    const int bonus = quality > 0 ? quality : 1;
    const bool better = bonus > AttackBonus();
    m_Items.push_back({profession == 0 ? "Runic gloves" : "Iron sword", bonus});
    if (better)
    {
        m_Equipped = static_cast<int>(m_Items.size()) - 1;
    }
}

int PlayerInventory::Gold() const
{
    return m_Gold;
}

int PlayerInventory::AttackBonus() const
{
    return m_Equipped < 0 ? 0 : m_Items[m_Equipped].attack;
}

int PlayerInventory::EquippedIndex() const
{
    return m_Equipped;
}

const std::vector<Equipment>& PlayerInventory::Items() const
{
    return m_Items;
}
