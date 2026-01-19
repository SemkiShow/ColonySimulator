// SPDX-FileCopyrightText: 2026 SemkiShow
//
// SPDX-License-Identifier: GPL-3.0-only

#pragma once

#include "UI/PopupPane.hpp"
#include <RWidgets/Layouts/RGridLayout.hpp>

class LoadMapMenu : public PopupPane
{
  public:
    LoadMapMenu();

    void Update() override;

    void ReloadSlots();

  private:
    std::shared_ptr<RGridLayout> slotLayout;
};

extern std::shared_ptr<LoadMapMenu> loadMapMenu;
