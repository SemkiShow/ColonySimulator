// SPDX-FileCopyrightText: 2026 SemkiShow
//
// SPDX-License-Identifier: GPL-3.0-only

#pragma once

#include <RWidgets/RPopupPane.hpp>
#include <RWidgets/RSlider.hpp>
#include <memory>

class SettingsMenu : public RPopupPane
{
  public:
    SettingsMenu();

    bool IsMusicVolumeSliderActive() { return musicVolumeSlider->IsSelected(); }

  private:
    std::shared_ptr<RSlider> musicVolumeSlider;
    std::shared_ptr<RSlider> sfxVolumeSlider;
};

extern std::shared_ptr<SettingsMenu> settingsMenu;
