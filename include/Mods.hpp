// SPDX-FileCopyrightText: 2026 SemkiShow
//
// SPDX-License-Identifier: GPL-3.0-only
/**
 * @file
 * @brief Lua modding support
 */

#pragma once

#ifdef LUA_BINDINGS

#include <filesystem>
#include <sol/state.hpp>

class ModLoader
{
  public:
    struct Mod
    {
        std::string name;
        sol::environment env;
        bool enabled = true;
        bool loaded = false;
    };

    ModLoader();
    ~ModLoader();

    void Save();
    void Load();
    void Update();

    std::vector<Mod> mods;

  private:
    sol::state lua;

    void CreateBindings();

    void LoadMod(const std::filesystem::path& path);
    void LoadMods();
};

extern std::shared_ptr<ModLoader> modLoader;

#endif
