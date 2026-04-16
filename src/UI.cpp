// SPDX-FileCopyrightText: 2025 SemkiShow
// SPDX-FileContributor: Jaraslau Zaitsau
//
// SPDX-License-Identifier: GPL-3.0-only

#include "UI.hpp"
#include "UI/AboutMenu.hpp"
#include "UI/DeleteSlotMenu.hpp"
#include "UI/EditIslandMenu.hpp"
#include "UI/GameMenu.hpp"
#include "UI/LoadMapMenu.hpp"
#include "UI/LoadingMenu.hpp"
#include "UI/MainMenu.hpp"
#include "UI/ModsMenu.hpp"
#include "UI/NewMapMenu.hpp"
#include "UI/PauseMenu.hpp"
#include "UI/SettingsMenu.hpp"
#include "UI/VictoryMenu.hpp"

std::shared_ptr<RApplication> app;

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

    editIslandMenu = std::make_shared<EditIslandMenu>();
    editIslandMenu->SetVisible(false);
    app->AddWindow(editIslandMenu);

    victoryMenu = std::make_shared<VictoryMenu>();
    victoryMenu->SetVisible(false);
    app->AddWindow(victoryMenu);

    pauseMenu = std::make_shared<PauseMenu>();
    pauseMenu->SetVisible(false);
    app->AddWindow(pauseMenu);

    settingsMenu = std::make_shared<SettingsMenu>();
    settingsMenu->SetVisible(false);
    app->AddWindow(settingsMenu);

    modsMenu = std::make_shared<ModsMenu>();
    modsMenu->SetVisible(false);
    app->AddWindow(modsMenu);

    loadingScreen = std::make_shared<LoadingScreen>();
    loadingScreen->SetVisible(false);
    app->AddWindow(loadingScreen);

    // app->SetDebugMode(true);
}
