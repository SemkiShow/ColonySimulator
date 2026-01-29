// SPDX-FileCopyrightText: 2026 SemkiShow
//
// SPDX-License-Identifier: GPL-3.0-only

#pragma once

#include <RWidgets/Windows/RPopupPane.hpp>

class AboutMenu : public RPopupPane
{
  public:
    AboutMenu();
};

extern std::shared_ptr<AboutMenu> aboutMenu;
