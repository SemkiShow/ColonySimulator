// SPDX-FileCopyrightText: 2026 SemkiShow
//
// SPDX-License-Identifier: GPL-3.0-only

#pragma once

#include <RWidgets/Windows/RWindow.hpp>

class GameMenu : public RWindow
{
  public:
    GameMenu();
};

extern std::shared_ptr<GameMenu> gameMenu;
