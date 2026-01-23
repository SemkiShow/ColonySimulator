// SPDX-FileCopyrightText: 2025 SemkiShow
//
// SPDX-License-Identifier: GPL-3.0-only

#include "Languages.hpp"
#include "Progress.hpp"
#include "UI.hpp"
#include <RCore/Translations.hpp>
#include <algorithm>
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
    std::sort(languages.begin(), languages.end());
}

void ReloadLabels()
{
    app->SetLanguage("ColonySimulator", "resources/locales", currentLanguage);

    for (auto& slot: saveSlots)
    {
        if (slot.seed == -1) slot.name = GetText("Empty slot");
    }

    UnloadSound(victorySound);
    if (currentLanguage == "pl")
    {
        victorySound = LoadSound("resources/sounds/gratulacje_uzytkowniku.mp3");
    }
    else
    {
        victorySound = LoadSound("resources/sounds/youve_got_mail.mp3");
    }
}
