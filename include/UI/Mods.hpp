// SPDX-FileCopyrightText: 2026 SemkiShow
//
// SPDX-License-Identifier: GPL-3.0-only
/**
 * @file
 * @brief The mods menu
 */

#pragma once

#include <RWidgets/RGridLayout.hpp>
#include <RWidgets/RPopupPane.hpp>

class ModsMenu : public RPopupPane
{
  public:
    ModsMenu();

    void ReloadModsList();

  private:
    std::shared_ptr<RGridLayout> modsLayout;
};

extern std::shared_ptr<ModsMenu> modsMenu;
