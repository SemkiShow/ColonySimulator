// SPDX-FileCopyrightText: 2026 SemkiShow
//
// SPDX-License-Identifier: GPL-3.0-only

#include "UI/Game.hpp"
#include "Drawing/Game.hpp"
#include "Island.hpp"
#include "Perlin.hpp"
#include "UI/EditIsland.hpp"

std::shared_ptr<GameMenu> gameMenu;

// TODO: Replace UI hardcoding with more RayUI usage
GameMenu::GameMenu()
{
    layout = std::make_shared<RLayout>();
    SetCentralWidget(layout);
    SetMargin(0);
    ReloadIslandUI();

    Connect([] { return true; }, [this] { UpdateIslandUI(); });
}

void GameMenu::Draw()
{
    DrawGameMenu();

    RWindow::Draw();

    DrawResources();
}

void GameMenu::UpdateIslandUI()
{
    float scale = 0.1f / perlinScale;

    for (auto& island: islands)
    {
        island.editButton->SetVisible(scale >= minScale && island.colonized);
    }

    // if (scale < minScale)
    // {
    //     for (auto& island: islands)
    //     {
    //         island.statsWidget->SetVisible(false);
    //     }
    //     return;
    // }

    // for (auto& island: islands)
    // {
    //     island.statsWidget->SetVisible(true);
    //     island.statsWidget->SetPosition(rui::FromRaylib(GlslToRaylib((island.p1 + island.p2) /
    //     2)) -
    //                                     island.statsWidget->GetSize() / 2);
    //     island.statsWidget->SetScale(scale);
    //     island.statsWidget->Update();
    // }
}

void GameMenu::ReloadIslandUI()
{
    layout->ClearWidgets();

    for (size_t i = 0; i < islands.size(); i++)
    {
        auto editButton = std::make_shared<RIconButton>(RIcon::Gear);
        layout->AddWidget(editButton);

        islands[i].editButton = editButton;

        std::weak_ptr<RIconButton> weakEditButton = editButton;

        Connect(
            [weakEditButton]
            {
                if (auto editButton = weakEditButton.lock()) return editButton->IsClicked();
                return false;
            },
            [i]
            {
                editIslandMenu->SetIslandIdx(i);
                editIslandMenu->SetVisible(true);
            },
            editButton);
    }

    // // Do not draw anything if the scale is too small
    // float scale = 0.1f / perlinScale;
    // if (scale < minScale) return;

    // const float lockHeight = 50, paneRadius = 3, imageHeight = 50, fontSize = 24,
    //             editButtonSize = 30;

    // for (auto& island: islands)
    // {
    //     auto vbox = std::make_shared<RVBoxLayout>();
    //     vbox->SetMargin(0);
    //     layout->AddWidget(vbox);

    //     if (!island.colonized)
    //     {
    //         auto lock = std::make_shared<RImage>(rui::FromRaylib(lockTexture));
    //         lock->SetAlignment(RAlign::HCenter);
    //         lock->SetMaxHeight(lockHeight);
    //         vbox->AddWidget(lock);
    //     }

    //     auto pane = std::make_shared<RPaneRounded>(paneRadius);
    //     pane->SetTint({0, 0, 0, 127});
    //     pane->SetMaxSize({100, 150});
    //     vbox->AddWidget(pane);

    //     auto hbox = std::make_shared<RHBoxLayout>();
    //     hbox->SetMargin(0);
    //     pane->SetCentralWidget(hbox);

    //     auto grid = std::make_shared<RGridLayout>(2);
    //     grid->SetMargin(0);
    //     hbox->AddWidget(grid);

    //     auto woodImage = std::make_shared<RImage>(rui::FromRaylib(woodTexture));
    //     woodImage->SetMaxWidth(imageHeight);
    //     woodImage->SetAlignment(RAlign::VCenter);
    //     grid->AddWidget(woodImage);

    //     auto woodLabel = std::make_shared<RLabel>(std::to_string(island.woodCount));
    //     woodLabel->SetFontSize(fontSize);
    //     woodLabel->SetTint({255, 255, 255});
    //     grid->AddWidget(woodLabel);

    //     auto editButton = std::make_shared<RIconButton>(RIcon::Gear);
    //     editButton->SetMaxSize({editButtonSize, editButtonSize});
    //     hbox->AddWidget(editButton);

    //     std::weak_ptr<RWidget> weakPane = pane, weakHbox = hbox, weakVbox = vbox;
    //     Connect([] { return true; },
    //             [weakPane, weakHbox, weakVbox]
    //             {
    //                 if (!weakPane.lock() || !weakHbox.lock() || !weakVbox.lock()) return;

    //                 auto pane = weakPane.lock();
    //                 auto hbox = weakHbox.lock();
    //                 auto vbox = weakVbox.lock();
    //                 pane->SetMaxSize(hbox->GetSize());
    //                 vbox->SetWidth(pane->GetWidth());
    //                 vbox->UpdateBounds();
    //                 vbox->Update();
    //             },
    //             pane, hbox, vbox);

    //     vbox->SetDebugMode(app->IsDebugMode());
    //     vbox->SetScale(scale);

    //     island.statsWidget = vbox;
    // }
    // layout->UpdateBounds();
    // layout->Update();
}
