// SPDX-FileCopyrightText: 2026 SemkiShow
//
// SPDX-License-Identifier: GPL-3.0-only

#include "UI/Settings.hpp"
#include "Languages.hpp"
#include "Settings.hpp"
#include <RCore/Translations.hpp>
#include <RWidgets/Buttons/RLabelButton.hpp>
#include <RWidgets/Checkboxes/RCheckbox.hpp>
#include <RWidgets/Labels/RLabel.hpp>
#include <RWidgets/Layouts/RGridLayout.hpp>
#include <RWidgets/Layouts/RHBoxLayout.hpp>
#include <RWidgets/Sliders/RSlider.hpp>
#include <raylib.h>

std::shared_ptr<SettingsMenu> settingsMenu;

SettingsMenu::SettingsMenu()
{
    auto layout = std::make_shared<RGridLayout>(2);
    layout->SetMargin(0);
    SetCentralWidget(layout);

    auto vsyncLabel = std::make_shared<RLabel>(_("vsync"));
    vsyncLabel->SetAlignment(RAlign::VCenter);
    layout->AddWidget(vsyncLabel);

    auto vsyncToggle = std::make_shared<RCheckbox>();
    vsyncToggle->SetValue(vsync);
    layout->AddWidget(vsyncToggle);

    Connect([vsyncToggle] { return vsyncToggle->IsClicked(); },
            [vsyncToggle]
            {
                vsync = vsyncToggle->GetValue();

                if (vsync)
                    SetWindowState(FLAG_VSYNC_HINT);
                else
                    ClearWindowState(FLAG_VSYNC_HINT);
            });

    auto fpsLabel = std::make_shared<RLabel>(_("show-fps"));
    fpsLabel->SetAlignment(RAlign::VCenter);
    layout->AddWidget(fpsLabel);

    auto fpsToggle = std::make_shared<RCheckbox>();
    fpsToggle->SetValue(showFPS);
    layout->AddWidget(fpsToggle);

    Connect([fpsToggle] { return fpsToggle->IsClicked(); },
            [fpsToggle] { showFPS = fpsToggle->GetValue(); });

    auto panLabel = std::make_shared<RLabel>(_("pan-sensitivity"));
    panLabel->SetAlignment(RAlign::VCenter);
    layout->AddWidget(panLabel);

    auto panSlider = std::make_shared<RSlider>(panSensitivity, 100, 1000, RSliderType::Rectangle);
    layout->AddWidget(panSlider);

    Connect([panSlider] { return panSlider->IsValueChanged(); },
            [panSlider] { panSensitivity = panSlider->GetValue(); });

    auto wheelLabel = std::make_shared<RLabel>(_("wheel-sensitivity"));
    wheelLabel->SetAlignment(RAlign::VCenter);
    layout->AddWidget(wheelLabel);

    auto wheelSlider =
        std::make_shared<RSlider>(wheelSensitivity, 0.05f, 10, RSliderType::Rectangle);
    layout->AddWidget(wheelSlider);

    Connect([wheelSlider] { return wheelSlider->IsValueChanged(); },
            [wheelSlider] { wheelSensitivity = wheelSlider->GetValue(); });

    auto langLabel = std::make_shared<RLabel>(_("language"));
    langLabel->SetAlignment(RAlign::VCenter);
    layout->AddWidget(langLabel);

    auto langLayout = std::make_shared<RHBoxLayout>();
    langLayout->SetMargin(0);
    for (auto& lang: languages)
    {
        auto button = std::make_shared<RLabelButton>(lang);
        button->SetMaxWidth(button->GetMinHeight());
        langLayout->AddWidget(button);

        Connect([button] { return button->IsClicked(); },
                [lang]
                {
                    currentLanguage = lang;
                    ReloadLabels();
                },
                button);
    }
    layout->AddWidget(langLayout);
}
