// SPDX-FileCopyrightText: 2025 SemkiShow
// SPDX-FileContributor: Jaraslau Zaitsau
//
// SPDX-License-Identifier: GPL-3.0-only

#include "UI.hpp"
#include "UI/About.hpp"
#include "UI/DeleteSlot.hpp"
#include "UI/LoadMap.hpp"
#include "UI/Loading.hpp"
#include "UI/Main.hpp"
#include "UI/NewMap.hpp"
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

    settingsMenu = std::make_shared<SettingsMenu>();
    settingsMenu->SetVisible(false);
    app->AddWindow(settingsMenu);

    aboutMenu = std::make_shared<AboutMenu>();
    aboutMenu->SetVisible(false);
    app->AddWindow(aboutMenu);

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

// void DrawGameUI()
// {
// if (showFPS) DrawFPS(0, 0);

// if (GuiButton(Rectangle{windowSize.x - ELEMENT_SIZE, 0, ELEMENT_SIZE, ELEMENT_SIZE},
// "#142#"))
//     isSettings = !isSettings;

// // if (GuiButton(Rectangle{windowSize.x - ELEMENT_SIZE * 2, 0, ELEMENT_SIZE, ELEMENT_SIZE},
// //               "#140#"))
// //     showIslandsBoxes = !showIslandsBoxes;

// if (islandEditIdx != -1) EditIsland();

// if (isSettings) DrawSettings();
// }

// void DrawPauseUI()
// {
// if (isSettings)
// {
//     DrawSettings();
//     return;
// }

// Rectangle rec = {UI_SPACING, UI_SPACING, windowSize.x - UI_SPACING * 2,
//                  windowSize.y - UI_SPACING * 2};
// DrawRectangleRounded(rec, 0.1f, 1, MENU_BACKGROUND);
// nextElementPositionY = rec.y + UI_SPACING;
// if (DrawButtonCentered(_("Return to game").c_str())) OpenGameMenu();
// if (DrawButtonCentered(_("Save game").c_str())) SaveProgress();
// if (DrawButtonCentered(_("Go to the main menu").c_str())) isSaveGame = true;

// if (isSaveGame)
// {
//     int res = GuiMessageBox(rec, _("Info").c_str(),
//                             _("Would you like to save the game before exiting?").c_str(),
//                             _("Yes;No").c_str());
//     if (res >= 0)
//     {
//         if (res != 1) LoadFromSlot(currentSlot, false);
//         SaveProgress();
//         currentMenu = Menu::Main;
//         isSaveGame = false;
//     }
// }
// }
