// SPDX-FileCopyrightText: 2026 SemkiShow
//
// SPDX-License-Identifier: GPL-3.0-only

#ifdef LUA_BINDINGS

#include "Mods.hpp"
#include "Bindings.hpp"
#include "JsonFormat.hpp"
#include <iostream>
#include <raylib.h>

std::shared_ptr<ModLoader> modLoader;

ModLoader::ModLoader()
{
    lua.open_libraries(sol::lib::base, sol::lib::package, sol::lib::math, sol::lib::table,
                       sol::lib::string);
    lua["os"] = sol::nil;
    lua["io"] = sol::nil;

    CreateBindings();

    Load();
    LoadMods();
}

ModLoader::~ModLoader() { mods.clear(); }

void ModLoader::Save()
{
    Json json;

    json["version"] = 0;

    for (auto& mod: mods)
    {
        json["mods"][mod.name] = mod.enabled;
    }

    if (!std::filesystem::exists("mods/")) std::filesystem::create_directory("mods");
    json.Save("mods/mods.json");
    std::cout << "Saved mods.json!\n";
}

void ModLoader::Load()
{
    if (!std::filesystem::exists("mods/mods.json"))
    {
        Save();
        return;
    }

    Json json = Json::Load("mods/mods.json");

    for (auto& modJson: json["mods"].GetObject())
    {
        mods.emplace_back();
        mods.back().name = modJson.first;
        mods.back().enabled = modJson.second.GetBool();
    }
}

void ModLoader::Update()
{
    for (auto& mod: mods)
    {
        if (!mod.enabled || !mod.loaded) continue;

        auto updateFunc = mod.env["Update"];

        if (!updateFunc.valid()) continue;

        auto result = updateFunc(GetFrameTime());

        if (!result.valid())
        {
            sol::error err = result;
            std::cerr << "Error in mod update: " << err.what() << "\n";
        }
    }
}

void ModLoader::CreateBindings() { CreateLuaBindings(lua); }

void ModLoader::LoadMod(const std::filesystem::path& path)
{
    auto getModName = [path]() -> std::string
    {
        const std::filesystem::path modsDir = "mods";
        if (path.parent_path() == modsDir)
            return path.stem().string();
        else
            return path.parent_path().stem().string();
    };

    for (auto& mod: mods)
    {
        if (mod.name != getModName()) continue;
        if (!mod.enabled) return;

        sol::environment env(lua, sol::create, lua.globals());
        mod.env = env;
        mod.loaded = true;

        // Setup mod path
        std::string modDirectory = path.parent_path().string();
        lua["package"]["path"] =
            lua["package"]["path"].get<std::string>() + ";" + modDirectory + "/?.lua";

        // Sanitize includes
        lua["package"]["searchers"][0] = [this](std::string moduleName) -> sol::object
        {
            std::replace(moduleName.begin(), moduleName.end(), '.', '/');

            std::string path = "mods/common/" + moduleName + ".lua";

            if (std::filesystem::exists(path))
            {
                auto load_result = lua.load_file(path);
                return load_result;
            }

            return sol::make_object(lua, "\n\t[VFS] No file found at " + path);
        };

        auto result = lua.script_file(path.string(), env, sol::script_pass_on_error);

        if (!result.valid())
        {
            sol::error err = result;
            std::cerr << "Failed to load mod " << path << ": " << err.what() << "\n";
            return;
        }

        std::cout << "Loaded mod " << getModName() << '\n';
        break;
    }
}

void ModLoader::LoadMods()
{
    std::filesystem::path modsDir("mods");

    if (!std::filesystem::exists(modsDir))
    {
        std::filesystem::create_directories(modsDir);
        return;
    }

    for (const auto& entry: std::filesystem::directory_iterator(modsDir))
    {
        if (entry.is_directory())
        {
            std::filesystem::path mainLua = entry.path() / "main.lua";
            if (std::filesystem::exists(mainLua)) LoadMod(mainLua);
        }
        else if (entry.is_regular_file() && entry.path().extension() == ".lua")
        {
            LoadMod(entry.path());
        }
    }
}

#endif
