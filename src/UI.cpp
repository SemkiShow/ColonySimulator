// SPDX-FileCopyrightText: 2025 SemkiShow
// SPDX-FileContributor: Jaraslau Zaitsau
//
// SPDX-License-Identifier: GPL-3.0-only

#include "UI.hpp"
#include "UI/About.hpp"
#include "UI/DeleteSlot.hpp"
#include "UI/EditIsland.hpp"
#include "UI/Game.hpp"
#include "UI/LoadMap.hpp"
#include "UI/Loading.hpp"
#include "UI/Main.hpp"
#include "UI/NewMap.hpp"
#include "UI/Pause.hpp"
#include "UI/Settings.hpp"
#include "UI/Victory.hpp"

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

    loadingScreen = std::make_shared<LoadingScreen>();
    loadingScreen->SetVisible(false);
    app->AddWindow(loadingScreen);

    // app->SetDebugMode(true);
}
