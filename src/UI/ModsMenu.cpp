// SPDX-FileCopyrightText: 2026 SemkiShow
//
// SPDX-License-Identifier: GPL-3.0-only

#include "UI/ModsMenu.hpp"
#include "Languages.hpp"
#include <RWidgets/RLabel.hpp>
#include <RWidgets/RScrollArea.hpp>
#include <RWidgets/RVBoxLayout.hpp>

#ifdef LUA_BINDINGS
#include "Drawing.hpp"
#include "Mods.hpp"
#include <RWidgets/RCheckbox.hpp>
#endif

std::shared_ptr<ModsMenu> modsMenu;

ModsMenu::ModsMenu()
{
    auto layout = std::make_shared<RVBoxLayout>();
    SetCentralWidget(layout);

    auto modsLabel = std::make_shared<RLabel>(_("Mods"), 32);
    modsLabel->SetAlignment(RAlign::HCenter);
    layout->AddWidget(modsLabel);

    auto reloadLabel = std::make_shared<RLabel>(
        _("Toggling mods may require restarting\nthe game for changes to take effect"),
        RColor{255, 255, 0});
    reloadLabel->SetAlignment(RAlign::HCenter);
    layout->AddWidget(reloadLabel);

    auto scrollArea = std::make_shared<RScrollArea>();
    layout->AddWidget(scrollArea);

    modsLayout = std::make_shared<RGridLayout>(2);
    modsLayout->SetAlignment(RAlign::HCenter);
    scrollArea->SetCentralWidget(modsLayout);
}

void ModsMenu::ReloadModsList()
{
    modsLayout->ClearWidgets();

#ifdef LUA_BINDINGS
    for (auto& mod: modLoader->mods)
    {
        auto checkbox = std::make_shared<RCheckbox>(mod.enabled);
        modsLayout->AddWidget(checkbox);

        std::weak_ptr<RCheckbox> weakCheckbox = checkbox;
        Connect(
            [weakCheckbox]
            {
                if (auto checkbox = weakCheckbox.lock()) return checkbox->IsClicked();
                return false;
            },
            [&mod, weakCheckbox]
            {
                if (auto checkbox = weakCheckbox.lock()) mod.enabled = checkbox->GetValue();
            },
            checkbox);

        auto label = std::make_shared<RLabel>(mod.name);
        label->SetFont(rayuiFont);
        modsLayout->AddWidget(label);
    }
#endif
}
