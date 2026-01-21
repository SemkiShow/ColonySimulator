// SPDX-FileCopyrightText: 2025 SemkiShow
// SPDX-FileContributor: Jaraslau Zaitsau
//
// SPDX-License-Identifier: GPL-3.0-only

#include "UI.hpp"
#include "UI/About.hpp"
#include "UI/DeleteSlot.hpp"
#include "UI/Game.hpp"
#include "UI/LoadMap.hpp"
#include "UI/Loading.hpp"
#include "UI/Main.hpp"
#include "UI/NewMap.hpp"
#include "UI/Pause.hpp"
#include "UI/Settings.hpp"
#include <raylib.h>

std::shared_ptr<RApplication> app;

int slotToEmpty = -1;
int newMapSlot = -1;
int slotSeed = -1;
bool squareMap = true;
Vector2 slotMapSize{300, 300};
int islandEditIdx = -1;

void InitUI()
{
    app = std::make_shared<RApplication>();

    mainMenu = std::make_shared<MainMenu>();
    app->AddWindow(mainMenu);

    loadMapMenu = std::make_shared<LoadMapMenu>();
    loadMapMenu->SetVisible(false);
    app->AddWindow(loadMapMenu);

    deleteSlotMenu = std::make_shared<DeleteSlotMenu>();
    deleteSlotMenu->SetVisible(false);
    app->AddWindow(deleteSlotMenu);

    newMapMenu = std::make_shared<NewMapMenu>();
    newMapMenu->SetVisible(false);
    app->AddWindow(newMapMenu);

    aboutMenu = std::make_shared<AboutMenu>();
    aboutMenu->SetVisible(false);
    app->AddWindow(aboutMenu);

    gameMenu = std::make_shared<GameMenu>();
    gameMenu->SetVisible(false);
    app->AddWindow(gameMenu);

    pauseMenu = std::make_shared<PauseMenu>();
    pauseMenu->SetVisible(false);
    app->AddWindow(pauseMenu);

    settingsMenu = std::make_shared<SettingsMenu>();
    settingsMenu->SetVisible(false);
    app->AddWindow(settingsMenu);

    loadingScreen = std::make_shared<LoadingScreen>();
    loadingScreen->SetVisible(false);
    app->AddWindow(loadingScreen);

    app->SetDebugMode(true);
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
