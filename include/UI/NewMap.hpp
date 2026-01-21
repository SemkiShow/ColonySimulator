// SPDX-FileCopyrightText: 2026 SemkiShow
//
// SPDX-License-Identifier: GPL-3.0-only

#pragma once

#include "UI/PopupPane.hpp"
#include <RWidgets/Sliders/RSlider.hpp>

class NewMapMenu : public PopupPane
{
  public:
    NewMapMenu();

  private:
    std::shared_ptr<RSliderInt> mapSizeXSlider, mapSizeYSlider;
};

extern std::shared_ptr<NewMapMenu> newMapMenu;
