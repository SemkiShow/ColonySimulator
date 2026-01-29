// SPDX-FileCopyrightText: 2026 SemkiShow
//
// SPDX-License-Identifier: GPL-3.0-only

#include "UI/About.hpp"
#include "Languages.hpp"
#include <RWidgets/RLabel.hpp>
#include <RWidgets/RVBoxLayout.hpp>

std::shared_ptr<AboutMenu> aboutMenu;

AboutMenu::AboutMenu()
{
    auto layout = std::make_shared<RVBoxLayout>();
    SetCentralWidget(layout);

    auto title = std::make_shared<RLabel>(_("Colony Simulator"), 64);
    title->SetAlignment(RAlign::HCenter);
    layout->AddWidget(title);

    auto credits =
        std::make_shared<RLabel>(_("Lead Developer: SemkiShow\nDeveloper: jaraslauzaitsau"), 32);
    credits->SetAlignment(RAlign::HCenter);
    credits->SetAlignment(RAlign::VCenter);
    layout->AddWidget(credits);

    auto license = std::make_shared<RLabel>(_("This game is licensed under GPL v3.0"), 24);
    license->SetAlignment(RAlign::HCenter);
    license->SetAlignment(RAlign::Bottom);
    layout->AddWidget(license);
}
