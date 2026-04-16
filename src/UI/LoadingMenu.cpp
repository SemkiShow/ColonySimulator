// SPDX-FileCopyrightText: 2026 SemkiShow
//
// SPDX-License-Identifier: GPL-3.0-only

#include "UI/LoadingMenu.hpp"
#include <RWidgets/RHBoxLayout.hpp>
#include <RWidgets/RPane.hpp>

std::shared_ptr<LoadingScreen> loadingScreen;

LoadingScreen::LoadingScreen()
{
    SetMargin(0);

    auto centralWidget = std::make_shared<RPane>();
    centralWidget->SetTint({0, 0, 0});
    centralWidget->SetDrawBorder(false);
    SetCentralWidget(centralWidget);

    auto layout = std::make_shared<RHBoxLayout>();
    centralWidget->SetCentralWidget(layout);

    label = std::make_shared<RLabel>();
    label->SetTint({255, 255, 255});
    label->SetAlignment(RAlign::Bottom);
    layout->AddWidget(label);

    progressBar = std::make_shared<RProgressBar>(0, 0, 100);
    label->SetMaxHeight(progressBar->GetMaxHeight());
    progressBar->SetAlignment(RAlign::Right);
    progressBar->SetAlignment(RAlign::Bottom);
    layout->AddWidget(progressBar);
}
