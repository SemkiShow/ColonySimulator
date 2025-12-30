// SPDX-FileCopyrightText: 2025 SemkiShow
//
// SPDX-License-Identifier: GPL-3.0-only

#pragma once

#include <string>
#include <unordered_map>
#include <vector>

extern std::string currentLanguage;
extern std::vector<std::string> languages;
extern std::unordered_map<std::string, std::string> labels;

void GetAllLanguages();
void ReloadLabels();
