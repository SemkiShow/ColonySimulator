// SPDX-FileCopyrightText: 2026 SemkiShow
//
// SPDX-License-Identifier: GPL-3.0-only

#pragma once

#include <RWidgets/Windows/RWindow.hpp>

class PauseMenu : public RWindow
{
  public:
    PauseMenu();
};

extern std::shared_ptr<PauseMenu> pauseMenu;
