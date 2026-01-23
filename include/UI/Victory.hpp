// SPDX-FileCopyrightText: 2026 SemkiShow
//
// SPDX-License-Identifier: GPL-3.0-only

#pragma once

#include <RWidgets/Windows/RPopupWindow.hpp>

class VictoryMenu : public RPopupWindow
{
  public:
    VictoryMenu();

    void Show();
};

extern std::shared_ptr<VictoryMenu> victoryMenu;
