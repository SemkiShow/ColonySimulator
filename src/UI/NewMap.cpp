// SPDX-FileCopyrightText: 2026 SemkiShow
//
// SPDX-License-Identifier: GPL-3.0-only

#include "UI/NewMap.hpp"
#include "Languages.hpp"
#include "Map.hpp"
#include "Perlin.hpp"
#include "Progress.hpp"
#include "Settings.hpp"
#include "UI/LoadMap.hpp"
#include <RCore/Translations.hpp>
#include <RWidgets/Buttons/RLabelButton.hpp>
#include <RWidgets/Checkboxes/RCheckbox.hpp>
#include <RWidgets/Labels/RLabel.hpp>
#include <RWidgets/Layouts/RGridLayout.hpp>
#include <RWidgets/Layouts/RVBoxLayout.hpp>
#include <RWidgets/Textboxes/RTextbox.hpp>
#include <RWidgets/Textboxes/RTextboxInt.hpp>

std::shared_ptr<NewMapMenu> newMapMenu;

NewMapMenu::NewMapMenu()
{
    auto mainLayout = std::make_shared<RVBoxLayout>();
    SetCentralWidget(mainLayout);

    auto layout = std::make_shared<RGridLayout>(2);
    mainLayout->AddWidget(layout);

    auto mapNameLabel = std::make_shared<RLabel>(_("map name"));
    mapNameLabel->SetAlignment(RAlign::VCenter);
    layout->AddWidget(mapNameLabel);

    auto mapNameTextbox = std::make_shared<RTextbox>(_("New map"));
    layout->AddWidget(mapNameTextbox);

    auto seedLabel = std::make_shared<RLabel>(_("seed"));
    seedLabel->SetAlignment(RAlign::VCenter);
    layout->AddWidget(seedLabel);

    auto seedTextbox = std::make_shared<RTextboxInt>(_("Enter the map's seed"), rand());
    layout->AddWidget(seedTextbox);

    auto squareMapLabel = std::make_shared<RLabel>(_("square map"));
    squareMapLabel->SetAlignment(RAlign::VCenter);
    layout->AddWidget(squareMapLabel);

    auto squareMapCheckbox = std::make_shared<RCheckbox>(true);
    layout->AddWidget(squareMapCheckbox);

    auto mapSizeXLabel = std::make_shared<RLabel>(_("map size x"));
    mapSizeXLabel->SetAlignment(RAlign::VCenter);
    layout->AddWidget(mapSizeXLabel);

    mapSizeXSlider = std::make_shared<RSliderInt>(300, 50, 1000, RSliderType::Rectangle);
    layout->AddWidget(mapSizeXSlider);

    Connect([this] { return mapSizeXSlider->IsValueChanged(); },
            [squareMapCheckbox, this]
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
            [squareMapCheckbox, this]
            {
                if (!squareMapCheckbox->GetValue()) return;
                mapSizeXSlider->SetValue(mapSizeYSlider->GetValue());
            });

    auto createButton = std::make_shared<RLabelButton>(_("Create map"));
    createButton->SetAlignment(RAlign::Bottom);
    mainLayout->AddWidget(createButton);

    auto resetToDefault = [mapNameTextbox, seedTextbox, squareMapCheckbox, this]
    {
        mapNameTextbox->SetValue(GetText("New map"));
        seedTextbox->SetNumber(rand());
        squareMapCheckbox->SetValue(true);
        mapSizeXSlider->SetValue(300);
        mapSizeYSlider->SetValue(300);
    };

    Connect([this] { return IsCloseButtonClicked(); }, resetToDefault);
    resetToDefault();

    Connect([createButton] { return createButton->IsClicked(); },
            [seedTextbox, this, mapNameTextbox, resetToDefault]
            {
                perlinSeed = seedTextbox->GetNumber();
                mapSize = {static_cast<float>(mapSizeXSlider->GetValue()),
                           static_cast<float>(mapSizeYSlider->GetValue())};
                BuildMap();
                saveSlots.emplace_back();
                SaveToSlot(saveSlots.size() - 1);
                if (mapNameTextbox->GetValue().empty())
                    saveSlots.back().name = GetText("New map");
                else
                    saveSlots.back().name = mapNameTextbox->GetValue();
                SaveProgress();
                loadMapMenu->ReloadSlots();
                SetVisible(false);
                resetToDefault();
            });
}
