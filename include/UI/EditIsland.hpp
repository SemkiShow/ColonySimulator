// SPDX-FileCopyrightText: 2026 SemkiShow
//
// SPDX-License-Identifier: GPL-3.0-only

#pragma once

#include "Island.hpp"
#include <RWidgets/RPopupWindow.hpp>
#include <RWidgets/RSlider.hpp>

class EditIslandMenu : public RPopupWindow
{
  public:
    EditIslandMenu();

    void SetIslandIdx(int val)
    {
        islandIdx = val;
        taxesSlider->SetValue(islands[islandIdx].taxes);
    }

  private:
    int islandIdx = 0;
    std::shared_ptr<RSliderInt> taxesSlider;
};

extern std::shared_ptr<EditIslandMenu> editIslandMenu;
