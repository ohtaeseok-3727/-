#include "stdafx.h"
#include "GameUiControls.h"
#include "SkillLayout.h"
#include <stdexcept>
#include <utility>

void VerifyUiControls()
{
    using A = GameUiControls::Action;
    using W = GameUiControls::Window;
    const auto check = [](bool ok, const char* message)
    {
        if (!ok)
        {
            throw std::runtime_error(message);
        }
    };
    for (const auto size : {std::pair<int, int>{1280, 720},
                            std::pair<int, int>{960, 540},
                            std::pair<int, int>{640, 480}})
    {
        GameUiControls ui;
        GameUiControls::State state;
        const auto buttons = GameUiControls::Layout(size.first, size.second, state);
        check(buttons.size() == 20, "HUD must expose 12 actions and eight quick slots");
        for (const auto& b : buttons)
        {
            check(b.rect.x >= 0 && b.rect.y >= 0 && b.rect.x + b.rect.width <= size.first &&
                      b.rect.y + b.rect.height <= size.second,
                  "HUD button outside viewport");
            const int x = static_cast<int>(b.rect.x + b.rect.width * .5f);
            const int y = static_cast<int>(b.rect.y + b.rect.height * .5f);
            check(ui.MouseButton(true, x, y, size.first, size.second, state) == A::None,
                  "Press activated action");
            if (b.action == A::Interact)
            {
                check(ui.HoldingInteract(), "Held mouse did not provide interaction hold");
                ui.MouseMove(-10, -10, size.first, size.second, state);
                check(!ui.HoldingInteract(), "Interaction hold remained outside button");
                ui.MouseMove(x, y, size.first, size.second, state);
            }
            check(ui.MouseButton(false, x, y, size.first, size.second, state) == b.action,
                  "Release did not match displayed button");
            ui.MouseButton(true, x, y, size.first, size.second, state);
            check(ui.MouseButton(false, -10, -10, size.first, size.second, state) == A::None,
                  "Outside release activated action");
            ui.MouseButton(true, x, y, size.first, size.second, state);
            ui.Cancel();
            check(ui.MouseButton(false, x, y, size.first, size.second, state) == A::None,
                  "Cancelled press activated action");
        }
        state.window = W::Inventory;
        for (const auto& b : GameUiControls::Layout(size.first, size.second, state))
        {
            check(b.action < A::Quick0 || b.action > A::Quick7,
                  "Modal window leaked quick-slot activation");
        }
        state.window = W::None;
        state.paused = true;
        for (const auto& b : GameUiControls::Layout(size.first, size.second, state))
        {
            if (b.action == A::Attack || b.action == A::Interact || b.action == A::Health ||
                b.action == A::Mana)
            {
                check(!b.enabled, "Paused UI enabled gameplay action");
            }
        }
        state.paused = false;
        state.defeated = true;
        bool retry = false;
        for (const auto& b : GameUiControls::Layout(size.first, size.second, state))
        {
            retry = retry || (b.action == A::Retry && b.enabled);
        }
        check(retry, "Defeat has no mouse retry");
    }
    for (int height : {570, 600, 644, 720})
    {
        const auto layout = SkillLayout::Create(960, height, 6);
        check(layout.panelY + 350.f < layout.slotsY, "Skill window hides drag destination slots");
    }
}
