// SPDX-FileCopyrightText: 2026 SemkiShow
//
// SPDX-License-Identifier: GPL-3.0-only

#pragma once

#include <RWidgets/RWindow.hpp>

class MainMenu : public RWindow
{
  public:
    MainMenu();
};

extern std::shared_ptr<MainMenu> mainMenu;
