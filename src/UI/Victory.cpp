// SPDX-FileCopyrightText: 2026 SemkiShow
//
// SPDX-License-Identifier: GPL-3.0-only

#include "UI/Victory.hpp"
#include "Drawing.hpp"
#include "Languages.hpp"
#include <RWidgets/Buttons/RLabelButton.hpp>
#include <RWidgets/Labels/RLabel.hpp>
#include <RWidgets/Layouts/RVBoxLayout.hpp>
#include <raylib.h>

std::shared_ptr<VictoryMenu> victoryMenu;

VictoryMenu::VictoryMenu()
{
    auto layout = std::make_shared<RVBoxLayout>();
    SetCentralWidget(layout);

    auto label = std::make_shared<RLabel>(_("You won!"));
    label->SetAlignment(RAlign::HCenter);
    layout->AddWidget(label);

    auto okButton = std::make_shared<RLabelButton>(_("Ok"));
    okButton->SetAlignment(RAlign::HCenter);
    okButton->SetAlignment(RAlign::Bottom);
    layout->AddWidget(okButton);

    Connect([okButton] { return okButton->IsClicked(); }, [this] { SetVisible(false); });
}

void VictoryMenu::Show()
{
    SetVisible(true);
    PlaySound(victorySound);
}
