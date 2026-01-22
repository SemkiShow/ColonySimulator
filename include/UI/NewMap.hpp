// SPDX-FileCopyrightText: 2026 SemkiShow
//
// SPDX-License-Identifier: GPL-3.0-only

#pragma once

#include "UI/PopupPane.hpp"
#include <RWidgets/Checkboxes/RCheckbox.hpp>
#include <RWidgets/Sliders/RSlider.hpp>
#include <RWidgets/Textboxes/RTextbox.hpp>
#include <RWidgets/Textboxes/RTextboxInt.hpp>

class NewMapMenu : public PopupPane
{
  public:
    NewMapMenu();

    void ResetToDefault();

  private:
    std::shared_ptr<RTextbox> mapNameTextbox;
    std::shared_ptr<RTextboxInt> seedTextbox;
    std::shared_ptr<RCheckbox> squareMapCheckbox;
    std::shared_ptr<RSliderInt> mapSizeXSlider, mapSizeYSlider;
};

extern std::shared_ptr<NewMapMenu> newMapMenu;
