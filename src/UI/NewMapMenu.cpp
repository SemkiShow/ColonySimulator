// SPDX-FileCopyrightText: 2026 SemkiShow
//
// SPDX-License-Identifier: GPL-3.0-only

#include "UI/NewMapMenu.hpp"
#include "Languages.hpp"
#include "Map.hpp"
#include "Perlin.hpp"
#include "Progress.hpp"
#include "Settings.hpp"
#include "UI/LoadMapMenu.hpp"
#include <RCore/Translations.hpp>
#include <RWidgets/RGridLayout.hpp>
#include <RWidgets/RLabel.hpp>
#include <RWidgets/RLabelButton.hpp>
#include <RWidgets/RVBoxLayout.hpp>

std::shared_ptr<NewMapMenu> newMapMenu;

NewMapMenu::NewMapMenu()
{
    auto mainLayout = std::make_shared<RVBoxLayout>();
    mainLayout->SetMargin(0);
    SetCentralWidget(mainLayout);

    auto layout = std::make_shared<RGridLayout>(2);
    layout->SetMargin(0);
    mainLayout->AddWidget(layout);

    auto mapNameLabel = std::make_shared<RLabel>(_("map name"));
    mapNameLabel->SetAlignment(RAlign::VCenter);
    layout->AddWidget(mapNameLabel);

    mapNameTextbox = std::make_shared<RTextbox>(_("New map"));
    layout->AddWidget(mapNameTextbox);

    auto seedLabel = std::make_shared<RLabel>(_("seed"));
    seedLabel->SetAlignment(RAlign::VCenter);
    layout->AddWidget(seedLabel);

    seedTextbox = std::make_shared<RTextboxInt>(_("Enter the map's seed"), rand());
    layout->AddWidget(seedTextbox);

    auto squareMapLabel = std::make_shared<RLabel>(_("square map"));
    squareMapLabel->SetAlignment(RAlign::VCenter);
    layout->AddWidget(squareMapLabel);

    squareMapCheckbox = std::make_shared<RCheckbox>(true);
    layout->AddWidget(squareMapCheckbox);

    auto mapSizeXLabel = std::make_shared<RLabel>(_("map size x"));
    mapSizeXLabel->SetAlignment(RAlign::VCenter);
    layout->AddWidget(mapSizeXLabel);

    mapSizeXSlider = std::make_shared<RSliderInt>(300, 50, 1000, RSliderType::Rectangle);
    layout->AddWidget(mapSizeXSlider);

    Connect([this] { return mapSizeXSlider->IsValueChanged(); },
            [this]
            {
                if (!squareMapCheckbox->GetValue()) return;
                mapSizeYSlider->SetValue(mapSizeXSlider->GetValue());
            });

    auto mapSizeYLabel = std::make_shared<RLabel>(_("map size y"));
    mapSizeYLabel->SetAlignment(RAlign::VCenter);
    layout->AddWidget(mapSizeYLabel);

    mapSizeYSlider = std::make_shared<RSliderInt>(300, 50, 1000, RSliderType::Rectangle);
    layout->AddWidget(mapSizeYSlider);

    Connect([this] { return mapSizeYSlider->IsValueChanged(); },
            [this]
            {
                if (!squareMapCheckbox->GetValue()) return;
                mapSizeXSlider->SetValue(mapSizeYSlider->GetValue());
            });

    auto createButton = std::make_shared<RLabelButton>(_("Create map"));
    createButton->SetAlignment(RAlign::Bottom);
    mainLayout->AddWidget(createButton);

    Connect([this] { return IsCloseButtonClicked(); }, [this] { ResetToDefault(); });

    Connect([createButton] { return createButton->IsClicked(); },
            [this]
            {
                perlinSeed = seedTextbox->GetNumber();
                mapSize = {static_cast<float>(mapSizeXSlider->GetValue()),
                           static_cast<float>(mapSizeYSlider->GetValue())};
                BuildMap();
                saveSlots.emplace_back();
                currentSlot = saveSlots.size() - 1;
                SaveToSlot(currentSlot);
                if (mapNameTextbox->GetValue().empty())
                    saveSlots.back().name = GetText("New map");
                else
                    saveSlots.back().name = mapNameTextbox->GetValue();
                SaveProgress();
                loadMapMenu->ReloadSlots();
                SetVisible(false);
                ResetToDefault();
            });
}

void NewMapMenu::ResetToDefault()
{
    mapNameTextbox->SetValue(GetText("New map"));
    seedTextbox->SetNumber(rand());
    squareMapCheckbox->SetValue(true);
    mapSizeXSlider->SetValue(300);
    mapSizeYSlider->SetValue(300);
}
