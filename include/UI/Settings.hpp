// SPDX-FileCopyrightText: 2026 SemkiShow
//
// SPDX-License-Identifier: GPL-3.0-only

#pragma once

#include "UI/PopupPane.hpp"

class SettingsMenu : public PopupPane
{
  public:
    SettingsMenu();
};

extern std::shared_ptr<SettingsMenu> settingsMenu;
