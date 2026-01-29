// SPDX-FileCopyrightText: 2026 SemkiShow
//
// SPDX-License-Identifier: GPL-3.0-only

#pragma once

#include <RWidgets/RCheckbox.hpp>
#include <RWidgets/RPopupPane.hpp>
#include <RWidgets/RSlider.hpp>
#include <RWidgets/RTextbox.hpp>
#include <RWidgets/RTextboxInt.hpp>

class NewMapMenu : public RPopupPane
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
