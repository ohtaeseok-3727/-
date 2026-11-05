#include "stdafx.h"
#include "TradeSystem.h"
#include "GameUiTheme.h"
#include "HudTextLayout.h"
#include <algorithm>
#include <string>

void TradeSystem::Toggle()
{
    m_Open = !m_Open;
    CancelPress();
    m_Page = 0;
    m_Message.clear();
}

void TradeSystem::Close()
{
    m_Open = false;
    CancelPress();
}

bool TradeSystem::IsOpen() const
{
    return m_Open;
}

void TradeSystem::CancelPress()
{
    m_Pressed = -1;
}

int TradeSystem::Hit(int x, int y, int width, int height, int itemCount) const
{
    if (!m_Open || width < 584 || height < 440)
    {
        return -1;
    }
    const int left = (width - 560) / 2;
    const int top = (height - 400) / 2;
    if (x >= left + 20 && x < left + 260 && y >= top + 80 && y < top + 224)
    {
        return (y - top - 80) / 48;
    }
    if (x >= left + 290 && x < left + 540 && y >= top + 80 && y < top + 272)
    {
        const int index = m_Page * 4 + (y - top - 80) / 48;
        return index < itemCount ? 10 + index : -1;
    }
    if (y >= top + 300 && y < top + 340)
    {
        if (x >= left + 290 && x < left + 390)
        {
            return 3;
        }
        if (x >= left + 420 && x < left + 540)
        {
            return 4;
        }
    }
    return -1;
}

void TradeSystem::MouseButton(
    bool down, int x, int y, int width, int height, int profession, PlayerInventory& inventory)
{
    const int hit = Hit(x, y, width, height, static_cast<int>(inventory.Items().size()));
    if (down)
    {
        m_Pressed = hit;
        return;
    }
    const int pressed = m_Pressed;
    CancelPress();
    if (hit < 0 || hit != pressed)
    {
        return;
    }
    if (hit == 0)
    {
        m_Message = inventory.BuyPotion(false, HealthPrice)
                        ? "체력 물약을 구매했습니다. [7] 사용"
                        : "구매할 금화가 부족하거나 더 보관할 수 없습니다.";
    }
    else if (hit == 1)
    {
        m_Message = inventory.BuyPotion(true, ManaPrice)
                        ? "마나 물약을 구매했습니다. [8] 사용"
                        : "구매할 금화가 부족하거나 더 보관할 수 없습니다.";
    }
    else if (hit == 2 && inventory.Gold() >= EquipmentPrice)
    {
        inventory.AddEquipment(profession, EquipmentAttack);
        inventory.SpendGold(EquipmentPrice);
        m_Message = "장비를 구매했습니다. [I] 가방에서 비교·착용하세요.";
    }
    else if (hit == 2)
    {
        m_Message = "장비를 구매할 금화가 부족합니다.";
    }
    else if (hit == 3)
    {
        m_Page = (std::max)(0, m_Page - 1);
    }
    else if (hit == 4)
    {
        m_Page = (std::min)(m_Page + 1,
                            (std::max)(0, (static_cast<int>(inventory.Items().size()) - 1) / 4));
    }
    else if (hit >= 10)
    {
        const int index = hit - 10;
        const int attack = inventory.Items()[index].attack;
        const int price = (std::min)(attack, 1000000) * 5;
        m_Message = inventory.SellEquipment(index, (std::max)(0, price))
                        ? "장비를 판매했습니다. 착용 장비를 팔았다면 다시 선택하세요."
                        : "장비를 판매할 수 없습니다.";
        m_Page =
            (std::min)(m_Page, (std::max)(0, (static_cast<int>(inventory.Items().size()) - 1) / 4));
    }
}

void TradeSystem::Draw(Renderer& r, const PlayerInventory& inventory, int width, int height) const
{
    if (!m_Open)
    {
        return;
    }
    if (width < 584 || height < 440)
    {
        r.Text(20, 150, "상점을 보려면 창을 키우세요. [T 닫기]", GameUiTheme::Text());
        return;
    }
    const float left = (width - 560) * .5f;
    const float top = (height - 400) * .5f;
    r.Rect(0, 0, float(width), float(height), Color(0, 0, 0, .45f));
    GameUiTheme::Window(r, left, top, 560, 400, "상점  [T 닫기]");
    r.Text(
        left + 20, top + 55, "보유 금화 " + std::to_string(inventory.Gold()), GameUiTheme::Text());
    r.Text(left + 290, top + 55, "장비 판매", GameUiTheme::Text());
    const std::string buys[] = {
        "체력 물약  20 금화", "마나 물약  20 금화", "장비 공격 +5  60 금화"};
    for (int i = 0; i < 3; ++i)
    {
        GameUiTheme::Button(r, left + 20, top + 80 + i * 48, 240, 42, buys[i]);
    }
    for (int i = 0; i < 4; ++i)
    {
        const int index = m_Page * 4 + i;
        if (index >= static_cast<int>(inventory.Items().size()))
        {
            break;
        }
        const Equipment& item = inventory.Items()[index];
        GameUiTheme::Button(
            r,
            left + 290,
            top + 80 + i * 48,
            250,
            42,
            item.name + " +" + std::to_string(item.attack) + "  " +
                std::to_string((std::max)(0, (std::min)(item.attack, 1000000) * 5)) + " 금화" +
                (index == inventory.EquippedIndex() ? " [착용]" : ""));
    }
    GameUiTheme::Button(r, left + 290, top + 300, 100, 40, "이전 쪽");
    GameUiTheme::Button(r, left + 420, top + 300, 120, 40, "다음 쪽");
    r.Text(left + 20,
           top + 350,
           "보유 물약: 체력 " + std::to_string(inventory.HealthPotions()) + " / 마나 " +
               std::to_string(inventory.ManaPotions()),
           GameUiTheme::Text());
    r.Text(left + 20,
           top + 375,
           HudTextLayout::Fit(
               r, m_Message.empty() ? "상품이나 장비를 클릭하여 거래합니다." : m_Message, 520.f),
           GameUiTheme::Muted());
}
