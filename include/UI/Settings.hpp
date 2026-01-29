// SPDX-FileCopyrightText: 2026 SemkiShow
//
// SPDX-License-Identifier: GPL-3.0-only

#pragma once

#include <RWidgets/RPopupPane.hpp>

class SettingsMenu : public RPopupPane
{
  public:
    SettingsMenu();
};

extern std::shared_ptr<SettingsMenu> settingsMenu;
