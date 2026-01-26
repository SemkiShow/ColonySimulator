// SPDX-FileCopyrightText: 2026 SemkiShow
//
// SPDX-License-Identifier: GPL-3.0-only

#include "UI/LoadMap.hpp"
#include "Languages.hpp"
#include "Progress.hpp"
#include "UI/DeleteSlot.hpp"
#include "UI/Game.hpp"
#include "UI/Main.hpp"
#include "UI/NewMap.hpp"
#include <RWidgets/Buttons/RLabelButton.hpp>
#include <RWidgets/Labels/RLabel.hpp>
#include <RWidgets/Layouts/RGridLayout.hpp>
#include <RWidgets/Layouts/RVBoxLayout.hpp>
#include <RWidgets/ScrollAreas/RScrollArea.hpp>

std::shared_ptr<LoadMapMenu> loadMapMenu;

LoadMapMenu::LoadMapMenu()
{
    auto layout = std::make_shared<RVBoxLayout>();
    layout->SetMargin(0);
    SetCentralWidget(layout);

    auto scrollArea = std::make_shared<RScrollArea>();
    layout->AddWidget(scrollArea);

    slotLayout = std::make_shared<RGridLayout>(3);
    scrollArea->SetCentralWidget(slotLayout);

    auto newMapButton = std::make_shared<RLabelButton>(_("New map"));
    newMapButton->SetAlignment(RAlign::Bottom);
    layout->AddWidget(newMapButton);

    Connect([newMapButton] { return newMapButton->IsClicked(); },
            []
            {
                newMapMenu->ResetToDefault();
                newMapMenu->SetVisible(true);
            },
            newMapButton);
}

void LoadMapMenu::ReloadSlots()
{
    slotLayout->ClearWidgets();

    for (size_t i = 0; i < saveSlots.size(); i++)
    {
        auto playButton = std::make_shared<RIconButton>(RIcon::PlayerPlay);
        slotLayout->AddWidget(playButton);

        std::weak_ptr<RIconButton> weakPlayButton = playButton;
        Connect(
            [weakPlayButton]
            {
                if (auto btn = weakPlayButton.lock()) return btn->IsClicked();
                return false;
            },
            [i]
            {
                LoadFromSlot(i, true);
                mainMenu->SetVisible(false);
                loadMapMenu->SetVisible(false);
                gameMenu->SetVisible(true);
            },
            playButton);

        auto deleteButton = std::make_shared<RIconButton>(RIcon::Bin);
        slotLayout->AddWidget(deleteButton);

        std::weak_ptr<RIconButton> weakDeleteButton = deleteButton;
        Connect(
            [weakDeleteButton]
            {
                if (auto btn = weakDeleteButton.lock()) return btn->IsClicked();
                return false;
            },
            [i]
            {
                deleteSlotMenu->SetSlotIdx(i);
                deleteSlotMenu->SetVisible(true);
            },
            deleteButton);

        auto label = std::make_shared<RLabel>(saveSlots[i].name);
        label->SetFont(rayuiFont);
        label->SetAlignment(RAlign::VCenter);
        slotLayout->AddWidget(label);
    }

    UpdateBounds();
}

void LoadMapMenu::Update()
{
    PopupPane::Update();

    bool anyDeleted = false;
    int counter = 0;
    for (auto it = saveSlots.begin(); it != saveSlots.end(); counter++)
    {
        if (it->deleteLater)
        {
            it = saveSlots.erase(it);
            auto path = GetSlotPath(counter);
            if (std::filesystem::exists(path)) std::filesystem::remove(path);
            anyDeleted = true;
            continue;
        }
        ++it;
    }
    if (anyDeleted)
    {
        FixSaveIds();
        ReloadSlots();
    }
}
