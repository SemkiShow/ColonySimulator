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

// void EditIsland()
// {
// Rectangle rec = {UI_SPACING, windowSize.y / 2, windowSize.x - UI_SPACING * 2, windowSize.y /
// 3}; rec.y -= rec.height / 2; DrawRectangleRounded(rec, 0.1f, 1, MENU_BACKGROUND);
// nextElementPositionY = rec.y + UI_SPACING;

// {
//     auto buttonRec = rec;
//     buttonRec.width = ELEMENT_SIZE;
//     buttonRec.height = ELEMENT_SIZE;
//     buttonRec.x += rec.width - UI_SPACING;
//     if (GuiButton(buttonRec, "#113#"))
//     {
//         islandEditIdx = -1;
//         return;
//     }
// }

// auto& island = islands[islandEditIdx];
// DrawSliderInt("", _("Taxes").c_str(), &island.taxes, 0, 100);
// }
