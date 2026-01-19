// SPDX-FileCopyrightText: 2026 SemkiShow
//
// SPDX-License-Identifier: GPL-3.0-only

#include "UI/Main.hpp"
#include "Drawing.hpp"
#include "Languages.hpp"
#include "UI/About.hpp"
#include "UI/LoadMap.hpp"
#include "UI/Settings.hpp"
#include <RWidgets/Buttons/RLabelButton.hpp>
#include <RWidgets/Labels/RLabel.hpp>
#include <RWidgets/Layouts/RVBoxLayout.hpp>

std::shared_ptr<MainMenu> mainMenu;

MainMenu::MainMenu()
{
    auto layout = std::make_shared<RVBoxLayout>();
    SetCentralWidget(layout);

    auto title = std::make_shared<RLabel>(_("Colony Simulator"), 48);
    title->SetAlignment(RAlign::HCenter);
    title->SetTint({255, 255, 255});
    layout->AddWidget(title);

    auto buttonLayout = std::make_shared<RVBoxLayout>();
    buttonLayout->SetAlignment(RAlign::VCenter);
    buttonLayout->SetPadding(20);
    layout->AddWidget(buttonLayout);

    const float buttonWidth = 250;

    auto playButton = std::make_shared<RLabelButton>(_("Play"));
    playButton->SetMaxWidth(buttonWidth);
    playButton->SetAlignment(RAlign::HCenter);
    buttonLayout->AddWidget(playButton);

    Connect([playButton] { return playButton->IsClicked(); }, [] { loadMapMenu->SetVisible(true); },
            playButton);

    auto settingsButton = std::make_shared<RLabelButton>(_("Settings"));
    settingsButton->SetMaxWidth(buttonWidth);
    settingsButton->SetAlignment(RAlign::HCenter);
    buttonLayout->AddWidget(settingsButton);

    Connect([settingsButton] { return settingsButton->IsClicked(); },
            [] { settingsMenu->SetVisible(true); }, settingsButton);

    auto aboutButton = std::make_shared<RLabelButton>(_("About"));
    aboutButton->SetMaxWidth(buttonWidth);
    aboutButton->SetAlignment(RAlign::HCenter);
    buttonLayout->AddWidget(aboutButton);

    Connect([aboutButton] { return aboutButton->IsClicked(); }, [] { aboutMenu->SetVisible(true); },
            aboutButton);

    auto exitButton = std::make_shared<RLabelButton>(_("Exit"));
    exitButton->SetMaxWidth(buttonWidth);
    exitButton->SetAlignment(RAlign::HCenter);
    buttonLayout->AddWidget(exitButton);

    Connect([exitButton] { return exitButton->IsClicked(); }, [] { shouldClose = true; },
            exitButton);
}
