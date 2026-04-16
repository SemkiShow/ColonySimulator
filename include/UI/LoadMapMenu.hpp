// SPDX-FileCopyrightText: 2026 SemkiShow
//
// SPDX-License-Identifier: GPL-3.0-only

#pragma once

#include <RWidgets/RGridLayout.hpp>
#include <RWidgets/RPopupPane.hpp>

class LoadMapMenu : public RPopupPane
{
  public:
    LoadMapMenu();

    void Update() override;

    void ReloadSlots();

  private:
    std::shared_ptr<RGridLayout> slotLayout;
};

extern std::shared_ptr<LoadMapMenu> loadMapMenu;
