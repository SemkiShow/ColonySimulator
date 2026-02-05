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
    ModLoader();
    ~ModLoader() = default;

    void Update();

  private:
    sol::state lua;
    std::vector<sol::environment> mods;

    void CreateRayUIBindings();
    void CreateBindings();

    void LoadMod(const std::filesystem::path& path);
    void LoadMods();
};

extern std::shared_ptr<ModLoader> modLoader;

#endif
