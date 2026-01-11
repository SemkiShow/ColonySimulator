// SPDX-FileCopyrightText: 2025 SemkiShow
//
// SPDX-License-Identifier: GPL-3.0-only

#include "Languages.hpp"
#include "Progress.hpp"
#include <filesystem>

std::string currentLanguage = "en";
std::vector<std::string> languages;

void GetAllLanguages()
{
    languages.clear();
    for (auto& file: std::filesystem::directory_iterator("resources/locales"))
    {
        if (!file.is_directory()) continue;
        languages.push_back(file.path().stem().string());
    }
}

void ReloadLabels()
{
#ifdef _WIN32
    _putenv(("LANGUAGE=" + currentLanguage).c_str());
#else
    setenv("LANGUAGE", currentLanguage.c_str(), 1);
#endif
    bindtextdomain("ColonySimulator", "resources/locales");
    textdomain("ColonySimulator");

    for (auto& slot: saveSlots)
    {
        if (slot.seed == -1) slot.name = _("Empty slot");
    }
}
