// SPDX-FileCopyrightText: 2026 SemkiShow
//
// SPDX-License-Identifier: GPL-3.0-only

#pragma once

#include <RWidgets/Labels/RLabel.hpp>
#include <RWidgets/Windows/RWindow.hpp>

class PauseMenu : public RWindow
{
  public:
    PauseMenu();

    void UpdateTimeLabel();

  private:
    std::shared_ptr<RLabel> timeLabel;
};

extern std::shared_ptr<PauseMenu> pauseMenu;
