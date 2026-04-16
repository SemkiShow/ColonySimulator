// SPDX-FileCopyrightText: 2026 SemkiShow
//
// SPDX-License-Identifier: GPL-3.0-only

#include "UI/AboutMenu.hpp"
#include "Languages.hpp"
#include <RWidgets/RLabel.hpp>
#include <RWidgets/RScrollArea.hpp>
#include <RWidgets/RVBoxLayout.hpp>

std::shared_ptr<AboutMenu> aboutMenu;

AboutMenu::AboutMenu()
{
    auto layout = std::make_shared<RVBoxLayout>();
    SetCentralWidget(layout);

    auto title = std::make_shared<RLabel>(_("Colony Simulator"), 64);
    title->SetAlignment(RAlign::HCenter);
    layout->AddWidget(title);

    auto scrollArea = std::make_shared<RScrollArea>();
    layout->AddWidget(scrollArea);

    auto creditsLayout = std::make_shared<RVBoxLayout>();
    scrollArea->SetCentralWidget(creditsLayout);

    auto addLabel = [creditsLayout](const std::string& text, int textSize)
    {
        auto label = std::make_shared<RLabel>(text, textSize);
        label->SetAlignment(RAlign::HCenter);
        label->SetAlignment(RAlign::VCenter);
        creditsLayout->AddWidget(label);
    };

    addLabel(_("Programming:"), 48);
    addLabel(_("SemkiShow\njaraslauzaitsau"), 32);
    addLabel(_("Music:"), 48);
    addLabel(_("jaraslauzaitsau"), 32);

    auto license = std::make_shared<RLabel>(_("This game is licensed under GPL v3.0"), 24);
    license->SetAlignment(RAlign::HCenter);
    license->SetAlignment(RAlign::Bottom);
    layout->AddWidget(license);
}
