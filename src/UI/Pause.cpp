// SPDX-FileCopyrightText: 2026 SemkiShow
//
// SPDX-License-Identifier: GPL-3.0-only

#include "UI/Pause.hpp"
#include "Languages.hpp"
#include "Progress.hpp"
#include "UI/Game.hpp"
#include "UI/Main.hpp"
#include "UI/Settings.hpp"
#include <RWidgets/Buttons/RLabelButton.hpp>
#include <RWidgets/Layouts/RVBoxLayout.hpp>
#include <RWidgets/Panes/RPaneRounded.hpp>
#include <cassert>

std::shared_ptr<PauseMenu> pauseMenu;

PauseMenu::PauseMenu()
{
    auto pane = std::make_shared<RPaneRounded>();
    pane->SetTint({127, 127, 127, 127});
    pane->SetDrawBorder(false);
    SetCentralWidget(pane);

    auto layout = std::make_shared<RVBoxLayout>();
    layout->SetAlignment(RAlign::VCenter);
    layout->SetPadding(20);
    pane->SetCentralWidget(layout);

    timeLabel = std::make_shared<RLabel>("");
    timeLabel->SetAlignment(RAlign::HCenter);
    layout->AddWidget(timeLabel);

    Connect([] { return true; }, [this] { UpdateTimeLabel(); });

    const float buttonWidth = 250;

    auto back = std::make_shared<RLabelButton>(_("Return to game"));
    back->SetMaxWidth(buttonWidth);
    back->SetAlignment(RAlign::HCenter);
    layout->AddWidget(back);

    Connect([back] { return back->IsClicked(); },
            [this]
            {
                gameMenu->SetVisible(true);
                SetVisible(false);
            });

    auto settings = std::make_shared<RLabelButton>(_("Settings"));
    settings->SetMaxWidth(buttonWidth);
    settings->SetAlignment(RAlign::HCenter);
    layout->AddWidget(settings);

    Connect([settings] { return settings->IsClicked(); }, [] { settingsMenu->SetVisible(true); });

    auto save = std::make_shared<RLabelButton>(_("Save game"));
    save->SetMaxWidth(buttonWidth);
    save->SetAlignment(RAlign::HCenter);
    layout->AddWidget(save);

    Connect([save] { return save->IsClicked(); }, [] { SaveProgress(); });

    auto main = std::make_shared<RLabelButton>(_("Go to the main menu"));
    main->SetMaxWidth(buttonWidth);
    main->SetAlignment(RAlign::HCenter);
    layout->AddWidget(main);

    Connect([main] { return main->IsClicked(); },
            [this]
            {
                SaveProgress();
                gameMenu->SetVisible(false);
                mainMenu->SetVisible(true);
                SetVisible(false);
            });
}

void PauseMenu::UpdateTimeLabel()
{
    assert(IsSlotValid(currentSlot));
    timeLabel->SetLabel(_("Time elapsed: ") + FormatTime(saveSlots[currentSlot].time));
}
