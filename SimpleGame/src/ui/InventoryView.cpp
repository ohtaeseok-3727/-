#include "stdafx.h"
#include "InventoryView.h"
#include "GameUiTheme.h"
#include <algorithm>
#include <string>

namespace
{
    const int SlotsPerPage = 8;
    const int ColumnCount = 4;
    const float PanelWidth = 584.f;
    const float PanelHeight = 404.f;
    const float SlotWidth = 132.f;
    const float SlotHeight = 104.f;
    const float SlotGap = 8.f;

    void DrawSwordIcon(Renderer& renderer, float centerX, float centerY)
    {
        renderer.Quad({centerX - 3.f, centerY + 17.f},
                      {centerX + 3.f, centerY + 17.f},
                      {centerX + 7.f, centerY - 16.f},
                      {centerX, centerY - 23.f},
                      Color(.34f, .40f, .45f));
        renderer.Rect(centerX - 12.f, centerY + 11.f, 24.f, 4.f, Color(.50f, .36f, .14f));
        renderer.Quad({centerX - 5.f, centerY + 15.f},
                      {centerX + 5.f, centerY + 15.f},
                      {centerX + 10.f, centerY + 27.f},
                      {centerX - 10.f, centerY + 27.f},
                      Color(.23f, .15f, .10f));
    }

    void DrawRuneGloveIcon(Renderer& renderer, float centerX, float centerY)
    {
        const Color glove(.20f, .32f, .44f);
        renderer.Quad({centerX - 14.f, centerY + 20.f},
                      {centerX + 10.f, centerY + 20.f},
                      {centerX + 15.f, centerY - 8.f},
                      {centerX - 9.f, centerY - 14.f},
                      glove);
        renderer.Rect(centerX - 9.f, centerY - 25.f, 5.f, 15.f, glove);
        renderer.Rect(centerX - 2.f, centerY - 28.f, 5.f, 17.f, glove);
        renderer.Rect(centerX + 5.f, centerY - 23.f, 5.f, 14.f, glove);
        renderer.Ellipse(centerX + 3.f, centerY + 5.f, 6.f, 6.f, Color(.50f, .40f, .17f));
        renderer.Ellipse(centerX + 3.f, centerY + 5.f, 2.f, 2.f, Color(.08f, .16f, .23f));
    }
}

void InventoryView::Draw(Renderer& renderer,
                         const PlayerInventory& inventory,
                         int viewportWidth,
                         int viewportHeight) const
{
    if (!m_IsOpen)
    {
        return;
    }

    const float panelWidth = (std::min)(float(viewportWidth - 24), PanelWidth);
    const float panelHeight = (std::min)(float(viewportHeight - 48), PanelHeight);
    if (panelWidth < PanelWidth || panelHeight < PanelHeight)
    {
        renderer.Text(20, 150, "인벤토리를 보려면 창을 키우세요. [I]", GameUiTheme::Gold());
        return;
    }

    const float panelX = (viewportWidth - panelWidth) * .5f;
    const float panelY = (viewportHeight - panelHeight) * .5f;
    const int itemCount = static_cast<int>(inventory.Items().size());
    const int pageCount = (std::max)(1, (itemCount + SlotsPerPage - 1) / SlotsPerPage);
    const int page = (std::min)(m_Page, pageCount - 1);

    renderer.Rect(0, 0, float(viewportWidth), float(viewportHeight), Color(0, 0, 0, .45f));
    GameUiTheme::Window(renderer, panelX, panelY, PanelWidth, PanelHeight, "인벤토리");
    renderer.Text(
        panelX + 20.f, panelY + 55.f, "I 닫기   Page Up / Down 쪽 이동", GameUiTheme::Muted());

    const float goldX = panelX + 362.f;
    renderer.Rect(goldX, panelY + 36.f, 202.f, 29.f, GameUiTheme::Surface());
    renderer.Ellipse(goldX + 17.f, panelY + 50.f, 9.f, 9.f, Color(.55f, .40f, .16f));
    renderer.Ellipse(goldX + 17.f, panelY + 50.f, 5.f, 5.f, Color(.68f, .54f, .28f));
    renderer.Text(goldX + 32.f,
                  panelY + 55.f,
                  "보유금  " + std::to_string(inventory.Gold()),
                  GameUiTheme::Text());

    renderer.Text(panelX + 20.f,
                  panelY + 89.f,
                  "장비 가방  " + std::to_string(itemCount) + "개",
                  GameUiTheme::Text());
    renderer.Text(panelX + 350.f,
                  panelY + 89.f,
                  "쪽 " + std::to_string(page + 1) + " / " + std::to_string(pageCount),
                  GameUiTheme::Muted());

    const float slotsX = panelX + 20.f;
    const float slotsY = panelY + 108.f;
    for (int slot = 0; slot < SlotsPerPage; ++slot)
    {
        const int row = slot / ColumnCount;
        const int column = slot % ColumnCount;
        const float slotX = slotsX + column * (SlotWidth + SlotGap);
        const float slotY = slotsY + row * (SlotHeight + SlotGap);
        const int itemIndex = page * SlotsPerPage + slot;
        const bool hasItem = itemIndex < itemCount;
        const bool equipped = hasItem && itemIndex == inventory.EquippedIndex();
        renderer.Rect(slotX,
                      slotY,
                      SlotWidth,
                      SlotHeight,
                      equipped ? GameUiTheme::Gold() : GameUiTheme::Border());
        renderer.Rect(slotX + 2.f,
                      slotY + 2.f,
                      SlotWidth - 4.f,
                      SlotHeight - 4.f,
                      hasItem ? GameUiTheme::Surface() : GameUiTheme::Ink());
        if (hasItem && itemIndex == m_Selected)
        {
            renderer.Rect(slotX, slotY + SlotHeight - 4.f, SlotWidth, 4.f, GameUiTheme::Gold());
        }

        if (!hasItem)
        {
            renderer.Text(slotX + 43.f, slotY + 54.f, "비어 있음", GameUiTheme::Muted());
            continue;
        }

        const Equipment& equipment = inventory.Items()[itemIndex];
        DrawEquipmentIcon(renderer, equipment, slotX + SlotWidth * .5f, slotY + 29.f);
        renderer.Text(slotX + 14.f, slotY + 62.f, equipment.name, GameUiTheme::Text());
        renderer.Text(slotX + 14.f,
                      slotY + 84.f,
                      "공격 +" + std::to_string(equipment.attack),
                      Color(.43f, .59f, .73f));
        renderer.Text(slotX + 99.f, slotY + 84.f, "x1", GameUiTheme::Muted());
        if (equipped)
        {
            renderer.Text(slotX + 10.f, slotY + 15.f, "착용", GameUiTheme::Gold());
        }
    }

    if (m_Selected >= 0 && m_Selected < itemCount)
    {
        const int difference = inventory.Items()[m_Selected].attack - inventory.AttackBonus();
        renderer.Text(panelX + 20.f,
                      panelY + 355.f,
                      "선택 공격 " + std::to_string(inventory.Items()[m_Selected].attack) +
                          " / 현재 " + std::to_string(inventory.AttackBonus()) + " (차이 " +
                          (difference >= 0 ? "+" : "") + std::to_string(difference) + ")",
                      GameUiTheme::Text());
        renderer.Text(panelX + 20.f,
                      panelY + 380.f,
                      "선택한 장비를 다시 클릭하면 착용합니다.",
                      GameUiTheme::Gold());
    }
    else
    {
        renderer.Text(panelX + 20.f,
                      panelY + 376.f,
                      "장비를 클릭해 비교하고 다시 클릭해 착용합니다.",
                      GameUiTheme::Muted());
    }
}

void InventoryView::Toggle()
{
    m_Selected = -1;
    m_Page = 0;
    m_IsOpen = !m_IsOpen;
    CancelPress();
}

void InventoryView::Close()
{
    m_IsOpen = false;
    CancelPress();
}

void InventoryView::ChangePage(int direction, int itemCount)
{
    if (!m_IsOpen)
    {
        return;
    }

    CancelPress();
    m_Selected = -1;
    const int lastPage = (std::max)(0, (itemCount - 1) / SlotsPerPage);
    m_Page = (std::max)(0, (std::min)(lastPage, m_Page + direction));
}

void InventoryView::DrawEquipmentIcon(Renderer& renderer,
                                      const Equipment& equipment,
                                      float centerX,
                                      float centerY) const
{
    if (equipment.name == "철검")
    {
        DrawSwordIcon(renderer, centerX, centerY);
        return;
    }

    DrawRuneGloveIcon(renderer, centerX, centerY);
}

void InventoryView::CancelPress()
{
    m_Pressed = -1;
}

void InventoryView::MouseButton(
    bool down, int x, int y, int width, int height, PlayerInventory& inventory)
{
    int hit = -1;
    if (m_IsOpen && width >= 608 && height >= 452)
    {
        const float left = (width - PanelWidth) * .5f + 20.f;
        const float top = (height - PanelHeight) * .5f + 108.f;
        for (int slot = 0; slot < SlotsPerPage; ++slot)
        {
            const float sx = left + (slot % ColumnCount) * (SlotWidth + SlotGap);
            const float sy = top + (slot / ColumnCount) * (SlotHeight + SlotGap);
            const int index = m_Page * SlotsPerPage + slot;
            if (x >= sx && x < sx + SlotWidth && y >= sy && y < sy + SlotHeight &&
                index < static_cast<int>(inventory.Items().size()))
            {
                hit = index;
                break;
            }
        }
    }
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
    if (m_Selected == hit)
    {
        inventory.Equip(hit);
    }
    else
    {
        m_Selected = hit;
    }
}
