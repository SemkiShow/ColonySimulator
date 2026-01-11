// SPDX-FileCopyrightText: 2025 SemkiShow
//
// SPDX-License-Identifier: GPL-3.0-only

#pragma once

#include <libintl.h>
#include <string>
#include <vector>

#define _(s) std::string(gettext(s))

extern std::string currentLanguage;
extern std::vector<std::string> languages;

void GetAllLanguages();
void ReloadLabels();
