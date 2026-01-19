// SPDX-FileCopyrightText: 2026 SemkiShow
//
// SPDX-License-Identifier: GPL-3.0-only

#include "UI/LoadMap.hpp"
#include "Languages.hpp"
#include "Progress.hpp"
#include <RWidgets/Buttons/RLabelButton.hpp>
#include <RWidgets/Labels/RLabel.hpp>
#include <RWidgets/Layouts/RGridLayout.hpp>
#include <RWidgets/Layouts/RHBoxLayout.hpp>
#include <RWidgets/Layouts/RVBoxLayout.hpp>
#include <iostream>

std::shared_ptr<LoadMapMenu> loadMapMenu;

LoadMapMenu::LoadMapMenu()
{
    auto centralLayout = std::make_shared<RVBoxLayout>();
    SetCentralWidget(centralLayout);

    slotLayout = std::make_shared<RGridLayout>(3);
    centralLayout->AddWidget(slotLayout);

    auto buttonLayout = std::make_shared<RHBoxLayout>();
    centralLayout->AddWidget(buttonLayout);

    auto newMapButton = std::make_shared<RLabelButton>(_("New map"));
    newMapButton->SetAlignment(RAlign::Bottom);
    buttonLayout->AddWidget(newMapButton);

    Connect([newMapButton] { return newMapButton->IsClicked(); }, [] {}, newMapButton);
}

void LoadMapMenu::ReloadSlots()
{
    slotLayout->ClearWidgets();

    for (size_t i = 0; i < saveSlots.size(); i++)
    {
        auto playButton = std::make_shared<RIconButton>(RIcon::PlayerPlay);
        slotLayout->AddWidget(playButton);

        std::weak_ptr<RIconButton> weakPlayButton = playButton;
        Connect(
            [weakPlayButton]
            {
                if (auto btn = weakPlayButton.lock()) return btn->IsClicked();
                return false;
            },
            [i] { LoadFromSlot(i, true); }, playButton);

        auto deleteButton = std::make_shared<RIconButton>(RIcon::Bin);
        slotLayout->AddWidget(deleteButton);

        std::weak_ptr<RIconButton> weakDeleteButton = deleteButton;
        Connect(
            [weakDeleteButton]
            {
                if (auto btn = weakDeleteButton.lock()) return btn->IsClicked();
                return false;
            },
            [i]
            {
                saveSlots[i].deleteLater = true;
                std::cout << "Deleted save slot " << i << '\n';
            },
            deleteButton);

        auto label = std::make_shared<RLabel>(saveSlots[i].name);
        label->SetFont(rayuiFont);
        label->SetAlignment(RAlign::VCenter);
        slotLayout->AddWidget(label);
    }

    UpdateBounds();
}

void LoadMapMenu::Update()
{
    PopupPane::Update();

    bool anyDeleted = false;
    for (auto it = saveSlots.begin(); it != saveSlots.end();)
    {
        if (it->deleteLater)
        {
            it = saveSlots.erase(it);
            anyDeleted = true;
            continue;
        }
        ++it;
    }
    if (anyDeleted) ReloadSlots();
}

// void DrawLoadMap()
// {
// Rectangle rec = {UI_SPACING, UI_SPACING, windowSize.x - UI_SPACING * 2,
//                  windowSize.y - UI_SPACING * 2};
// DrawRectangleRounded(rec, 0.1f, 1, MENU_BACKGROUND);
// nextElementPositionY = rec.y + UI_SPACING;
// for (size_t i = 0; i < MAX_SAVE_SLOTS; i++)
// {
//     float posX = UI_SPACING * 2;
//     if (saveSlots[i].seed == -1)
//     {
//         // New map
//         if (GuiButton({posX, nextElementPositionY, BUTTON_SIZE, BUTTON_SIZE}, "+"))
//         {
//             newMapSlot = i;
//             slotSeed = rand();
//             squareMap = true;
//             slotMapSize = {300, 300};
//             isNewWorld = true;
//         }
//         posX += (BUTTON_SIZE + ELEMENT_SPACING) * 2;
//     }
//     else
//     {
//         // Load map
//         if (GuiButton({posX, nextElementPositionY, BUTTON_SIZE, BUTTON_SIZE}, "#131#"))
//         {
//             LoadFromSlot(i, true);
//             OpenGameMenu();
//             isLoadMap = false;
//         }
//         posX += BUTTON_SIZE + ELEMENT_SPACING;
//         // Delete map
//         if (GuiButton({posX, nextElementPositionY, BUTTON_SIZE, BUTTON_SIZE}, "#143#"))
//         {
//             isEmptySlot = true;
//             slotToEmpty = i;
//         }
//         posX += BUTTON_SIZE + ELEMENT_SPACING;
//     }
//     DrawTextCustom(saveSlots[i].name.c_str(), {posX, nextElementPositionY}, FONT_SIZE,
//     WHITE); nextElementPositionY += BUTTON_SIZE + ELEMENT_SPACING;
// }

// {
//     auto buttonRec = rec;
//     buttonRec.width = ELEMENT_SIZE * windowSize.x / startWindowSize.x;
//     buttonRec.height = ELEMENT_SIZE * windowSize.y / startWindowSize.y;
//     buttonRec.x += rec.width - buttonRec.width;
//     if (GuiButton(buttonRec, "#113#")) isLoadMap = false;
// }

// if (isEmptySlot)
// {
//     int res = GuiMessageBox(
//         rec, _("Warning").c_str(),
//         (_("Are you sure you want to empty") + " " + saveSlots[slotToEmpty].name +
//         "?").c_str(),
//         _("Yes;No").c_str());
//     if (res >= 0)
//     {
//         if (res == 1) EmptySlot(slotToEmpty);
//         isEmptySlot = false;
//         slotToEmpty = -1;
//     }
// }
// }
