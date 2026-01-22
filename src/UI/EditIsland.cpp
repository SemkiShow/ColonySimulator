// SPDX-FileCopyrightText: 2026 SemkiShow
//
// SPDX-License-Identifier: GPL-3.0-only

#include "UI/EditIsland.hpp"
#include "Languages.hpp"
#include <RWidgets/Labels/RLabel.hpp>
#include <RWidgets/Layouts/RGridLayout.hpp>

std::shared_ptr<EditIslandMenu> editIslandMenu;

EditIslandMenu::EditIslandMenu()
{
    auto layout = std::make_shared<RGridLayout>(2);
    SetCentralWidget(layout);

    auto taxesLabel = std::make_shared<RLabel>(_("Taxes"));
    taxesLabel->SetAlignment(RAlign::VCenter);
    layout->AddWidget(taxesLabel);

    taxesSlider = std::make_shared<RSliderInt>(0, 0, 100, RSliderType::Rectangle);
    layout->AddWidget(taxesSlider);

    Connect([this] { return taxesSlider->IsValueChanged(); },
            [this] { islands[islandIdx].taxes = taxesSlider->GetValue(); });
}
