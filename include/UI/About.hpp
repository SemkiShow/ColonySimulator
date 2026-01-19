// SPDX-FileCopyrightText: 2026 SemkiShow
//
// SPDX-License-Identifier: GPL-3.0-only

#pragma once

#include "UI/PopupPane.hpp"

class AboutMenu : public PopupPane
{
  public:
    AboutMenu();
};

extern std::shared_ptr<AboutMenu> aboutMenu;
