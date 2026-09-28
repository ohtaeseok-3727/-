#include "stdafx.h"
#include "InventoryView.h"
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
                      Color(.78f, .85f, .91f));
        renderer.Rect(centerX - 12.f, centerY + 11.f, 24.f, 4.f, Color(.92f, .71f, .26f));
        renderer.Quad({centerX - 5.f, centerY + 15.f},
                      {centerX + 5.f, centerY + 15.f},
                      {centerX + 10.f, centerY + 27.f},
                      {centerX - 10.f, centerY + 27.f},
                      Color(.33f, .20f, .12f));
    }

    void DrawRuneGloveIcon(Renderer& renderer, float centerX, float centerY)
    {
        const Color glove(.40f, .66f, .88f);
        renderer.Quad({centerX - 14.f, centerY + 20.f},
                      {centerX + 10.f, centerY + 20.f},
                      {centerX + 15.f, centerY - 8.f},
                      {centerX - 9.f, centerY - 14.f},
                      glove);
        renderer.Rect(centerX - 9.f, centerY - 25.f, 5.f, 15.f, glove);
        renderer.Rect(centerX - 2.f, centerY - 28.f, 5.f, 17.f, glove);
        renderer.Rect(centerX + 5.f, centerY - 23.f, 5.f, 14.f, glove);
        renderer.Ellipse(centerX + 3.f, centerY + 5.f, 6.f, 6.f, Color(.90f, .77f, .31f));
        renderer.Ellipse(centerX + 3.f, centerY + 5.f, 2.f, 2.f, Color(.12f, .24f, .36f));
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
    const float panelHeight = (std::min)(float(viewportHeight - 170), PanelHeight);
    if (panelWidth < PanelWidth || panelHeight < PanelHeight)
    {
        renderer.Text(20,
                      150,
                      "인벤토리를 보려면 창을 키우세요. [I]",
                      Color(1.f, .8f, .5f));
        return;
    }

    const float panelX = (viewportWidth - panelWidth) * .5f;
    const float panelY = 128.f;
    const int itemCount = static_cast<int>(inventory.Items().size());
    const int pageCount = (std::max)(1, (itemCount + SlotsPerPage - 1) / SlotsPerPage);
    const int page = (std::min)(m_Page, pageCount - 1);

    renderer.Rect(panelX, panelY, PanelWidth, PanelHeight, Color(.025f, .045f, .055f, .98f));
    renderer.Rect(panelX + 4.f, panelY + 4.f, PanelWidth - 8.f, 3.f, Color(.80f, .63f, .25f));
    renderer.Text(panelX + 20.f, panelY + 27.f, "인벤토리", Color(.91f, .83f, .63f));
    renderer.Text(panelX + 20.f,
                  panelY + 51.f,
                  "I 닫기   Page Up / Down 쪽 이동",
                  Color(.58f, .70f, .75f));

    const float goldX = panelX + 362.f;
    renderer.Rect(goldX, panelY + 18.f, 202.f, 47.f, Color(.10f, .085f, .04f, .92f));
    renderer.Ellipse(goldX + 24.f, panelY + 41.f, 10.f, 10.f, Color(.96f, .72f, .22f));
    renderer.Ellipse(goldX + 24.f, panelY + 41.f, 5.f, 5.f, Color(1.f, .88f, .45f));
    renderer.Text(goldX + 43.f,
                  panelY + 39.f,
                  "보유금  " + std::to_string(inventory.Gold()),
                  Color(.96f, .80f, .36f));

    renderer.Text(panelX + 20.f,
                  panelY + 89.f,
                  "장비 가방  " + std::to_string(itemCount) + "개",
                  Color(.72f, .82f, .88f));
    renderer.Text(panelX + 435.f,
                  panelY + 89.f,
                  "쪽 " + std::to_string(page + 1) + " / " + std::to_string(pageCount),
                  Color(.68f, .75f, .70f));

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
        const Color slotColor = equipped ? Color(.18f, .24f, .22f, .98f)
                            : hasItem ? Color(.075f, .11f, .13f, .98f)
                                      : Color(.045f, .068f, .078f, .92f);

        renderer.Rect(slotX, slotY, SlotWidth, SlotHeight, slotColor);
        renderer.Rect(slotX, slotY, SlotWidth, 2.f, equipped ? Color(.96f, .73f, .28f)
                                                             : Color(.18f, .28f, .32f));

        if (!hasItem)
        {
            renderer.Text(slotX + 43.f, slotY + 54.f, "비어 있음", Color(.32f, .42f, .46f));
            continue;
        }

        const Equipment& equipment = inventory.Items()[itemIndex];
        DrawEquipmentIcon(renderer, equipment, slotX + SlotWidth * .5f, slotY + 29.f);
        renderer.Text(slotX + 14.f, slotY + 62.f, equipment.name, Color(.83f, .89f, .89f));
        renderer.Text(slotX + 14.f,
                      slotY + 84.f,
                      "공격 +" + std::to_string(equipment.attack),
                      Color(.62f, .78f, .88f));
        renderer.Text(slotX + 99.f, slotY + 84.f, "x1", Color(.74f, .79f, .72f));
        if (equipped)
        {
            renderer.Text(slotX + 10.f, slotY + 15.f, "착용", Color(.98f, .77f, .31f));
        }
    }

    renderer.Text(panelX + 20.f,
                  panelY + 376.f,
                  "획득한 장비 중 공격력이 가장 높은 장비를 자동 착용합니다.",
                  Color(.59f, .70f, .67f));
}

void InventoryView::Toggle()
{
    m_IsOpen = !m_IsOpen;
}

void InventoryView::Close()
{
    m_IsOpen = false;
}

void InventoryView::ChangePage(int direction, int itemCount)
{
    if (!m_IsOpen)
    {
        return;
    }

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
