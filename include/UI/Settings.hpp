// SPDX-FileCopyrightText: 2026 SemkiShow
//
// SPDX-License-Identifier: GPL-3.0-only

#pragma once

#include "UI/PopupPane.hpp"
#include <RWidgets/Checkboxes/RCheckbox.hpp>
#include <RWidgets/Sliders/RSlider.hpp>

class SettingsMenu : public PopupPane
{
  public:
    SettingsMenu();

    void Update() override;

  private:
    std::shared_ptr<RCheckbox> vsyncToggle, fpsToggle;
    std::shared_ptr<RSlider> panSlider, wheelSlider;
};

extern std::shared_ptr<SettingsMenu> settingsMenu;
