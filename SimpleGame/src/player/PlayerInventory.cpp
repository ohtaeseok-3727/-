#include "stdafx.h"
#include "PlayerInventory.h"
#include <limits>

void PlayerInventory::Reset()
{
    m_Gold = 0;
    m_Equipped = -1;
    m_Items.clear();
    m_HealthPotions = 0;
    m_ManaPotions = 0;
}

void PlayerInventory::AddGold(int amount)
{
    if (amount > 0)
    {
        m_Gold += (std::min)(amount, (std::numeric_limits<int>::max)() - m_Gold);
    }
}

bool PlayerInventory::SpendGold(int amount)
{
    if (amount < 0 || m_Gold < amount)
    {
        return false;
    }
    m_Gold -= amount;
    return true;
}

void PlayerInventory::AddEquipment(int profession, int quality)
{
    const int bonus = quality > 0 ? quality : 1;
    const bool better = bonus > AttackBonus();
    m_Items.push_back({profession == 0 ? "룬 장갑" : "철검", bonus});
    if (better)
    {
        m_Equipped = static_cast<int>(m_Items.size()) - 1;
    }
}

bool PlayerInventory::Equip(int index)
{
    if (index < 0 || index >= static_cast<int>(m_Items.size()))
    {
        return false;
    }
    m_Equipped = index;
    return true;
}

bool PlayerInventory::SellEquipment(int index, int price)
{
    if (index < 0 || index >= static_cast<int>(m_Items.size()) || price < 0 ||
        price > (std::numeric_limits<int>::max)() - m_Gold)
    {
        return false;
    }
    m_Items.erase(m_Items.begin() + index);
    if (m_Equipped == index)
    {
        m_Equipped = -1;
    }
    else if (m_Equipped > index)
    {
        --m_Equipped;
    }
    m_Gold += price;
    return true;
}

bool PlayerInventory::BuyPotion(bool mana, int price)
{
    int& count = mana ? m_ManaPotions : m_HealthPotions;
    if (count == (std::numeric_limits<int>::max)() || !SpendGold(price))
    {
        return false;
    }
    ++count;
    return true;
}

bool PlayerInventory::UsePotion(bool mana)
{
    int& count = mana ? m_ManaPotions : m_HealthPotions;
    if (count <= 0)
    {
        return false;
    }
    --count;
    return true;
}

int PlayerInventory::HealthPotions() const
{
    return m_HealthPotions;
}

int PlayerInventory::ManaPotions() const
{
    return m_ManaPotions;
}

int PlayerInventory::Gold() const
{
    return m_Gold;
}

int PlayerInventory::AttackBonus() const
{
    return m_Equipped < 0 || m_Equipped >= static_cast<int>(m_Items.size())
               ? 0
               : m_Items[m_Equipped].attack;
}

int PlayerInventory::EquippedIndex() const
{
    return m_Equipped;
}

const std::vector<Equipment>& PlayerInventory::Items() const
{
    return m_Items;
}
