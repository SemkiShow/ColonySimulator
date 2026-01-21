// SPDX-FileCopyrightText: 2026 SemkiShow
//
// SPDX-License-Identifier: GPL-3.0-only

#pragma once

#include <RWidgets/Layouts/RLayout.hpp>
#include <RWidgets/Windows/RWindow.hpp>

class GameMenu : public RWindow
{
  public:
    GameMenu();

    void UpdateIslandUI();
    void ReloadIslandUI();

  private:
    std::shared_ptr<RLayout> layout;
    const float minScale = 0.5f;
};

extern std::shared_ptr<GameMenu> gameMenu;
