// SPDX-FileCopyrightText: 2026 SemkiShow
//
// SPDX-License-Identifier: GPL-3.0-only

#pragma once

#include <RWidgets/RLabel.hpp>
#include <RWidgets/RPopupWindow.hpp>

class VictoryMenu : public RPopupWindow
{
  public:
    VictoryMenu();

    void Show();

  private:
    std::shared_ptr<RLabel> label;
};

extern std::shared_ptr<VictoryMenu> victoryMenu;
