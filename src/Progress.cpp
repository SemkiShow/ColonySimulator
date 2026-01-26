// SPDX-FileCopyrightText: 2025 SemkiShow
//
// SPDX-License-Identifier: GPL-3.0-only

#include "Progress.hpp"
#include "Drawing.hpp"
#include "Human.hpp"
#include "Island.hpp"
#include "Languages.hpp"
#include "Map.hpp"
#include "Perlin.hpp"
#include "Settings.hpp"
#include "Ship.hpp"
#include "UI/Game.hpp"
#include "UI/LoadMap.hpp"
#include "UI/Loading.hpp"
#include <ctime>
#include <filesystem>
#include <iostream>

std::vector<SaveSlot> saveSlots;
int currentSlot = -1;

Json SaveSlot::ToJSON()
{
    Json json;

    json["seed"] = seed;
    json["name"] = name;
    for (auto& island: this->islands)
    {
        json["islands"].push_back(island.ToJSON());
    }
    for (auto& ship: this->ships)
    {
        json["ships"].push_back(ship.ToJSON());
    }
    for (auto& human: this->people)
    {
        json["people"].push_back(human.ToJSON());
    }
    json["woodTotal"] = this->woodTotal;
    json["ironTotal"] = this->ironTotal;
    json["peopleTotal"] = this->peopleTotal;
    json["mapSize"] = Vector2ToJson(this->mapSize);
    json["completed"] = this->completed;
    json["time"] = this->time;
    return json;
}

void SaveSlot::LoadJSON(Json& json)
{
    seed = json["seed"].GetDouble();
    name = json["name"].GetString();
    this->islands.clear();
    for (size_t i = 0; i < json["islands"].size(); i++)
    {
        this->islands.push_back(Island::LoadJSON(json["islands"][i]));
        this->islands.back().index = i;
    }
    this->ships.clear();
    for (size_t i = 0; i < json["ships"].size(); i++)
    {
        this->ships.push_back(Ship::LoadJSON(json["ships"][i]));
    }
    this->people.clear();
    for (size_t i = 0; i < json["people"].size(); i++)
    {
        this->people.push_back(Human::LoadJSON(json["people"][i]));
    }
    this->woodTotal = json["woodTotal"].GetInt();
    this->ironTotal = json["ironTotal"].GetInt();
    this->peopleTotal = json["peopleTotal"].GetInt();
    this->mapSize = JsonToVector2(json["mapSize"]);
    this->completed = json["completed"].GetBool();
    this->time = json["time"].GetDouble();
}

bool IsSlotValid(int idx) { return idx >= 0 && idx < (int)saveSlots.size(); }

void SaveToSlot(int idx)
{
    if (!IsSlotValid(idx))
    {
        std::cerr << "Invalid save slot index " << idx << '\n';
        return;
    }
    saveSlots[idx].seed = perlinSeed;
    saveSlots[idx].islands = islands;
    saveSlots[idx].ships = ships;
    saveSlots[idx].people = people;
    saveSlots[idx].woodTotal = woodTotal;
    saveSlots[idx].ironTotal = ironTotal;
    saveSlots[idx].peopleTotal = peopleTotal;
    saveSlots[idx].mapSize = mapSize;
    saveSlots[idx].opened = true;
}

void LoadFromSlot(int idx, bool generatePathMap)
{
    if (!IsSlotValid(idx))
    {
        std::cerr << "Invalid save slot index " << idx << '\n';
        return;
    }

    currentSlot = idx;
    if (saveSlots[idx].seed == -1)
    {
        BuildMap();
        SaveToSlot(idx);
        return;
    }

    auto loadSlot = [idx](std::string& label, float& loadingPercent, std::atomic<bool>& finished)
    {
        label = _("Loading progress...");
        loadingPercent = 0;
        saveSlots[idx] = LoadFile(GetSlotPath(idx));
        saveSlots[idx].opened = true;
        finished = true;
    };
    ShowLoadingScreen(false, loadSlot);

    perlinSeed = saveSlots[idx].seed;
    islands = saveSlots[idx].islands;
    ships = saveSlots[idx].ships;
    people = saveSlots[idx].people;
    woodTotal = saveSlots[idx].woodTotal;
    ironTotal = saveSlots[idx].ironTotal;
    peopleTotal = saveSlots[idx].peopleTotal;
    mapSize = saveSlots[idx].mapSize;

    SetShaderValue(perlinShader, GetShaderLocation(perlinShader, "uSeed"), &perlinSeed,
                   SHADER_UNIFORM_INT);

    if (!generatePathMap) return;

    auto func = [](std::string& label, float& loadingPercent, std::atomic<bool>& finished)
    {
        label = "Node graph out of date. Rebuilding...";
        GeneratePathMap(loadingPercent);
        finished = true;
    };
    ShowLoadingScreen(true, func);

    gameMenu->ReloadIslandUI();
}

void FixSaveIds()
{
    int id = 0;
    for (auto& file: std::filesystem::directory_iterator("saves/"))
    {
        if (!file.is_regular_file()) continue;

        std::filesystem::rename(file, GetSlotPath(id++));
    }
}

std::string GetSlotPath(int idx) { return "saves/" + std::to_string(idx) + ".json"; }

void SaveProgress()
{
    SaveToSlot(currentSlot);

    if (!std::filesystem::exists("saves/")) std::filesystem::create_directory("saves");

    FixSaveIds();
    for (size_t i = 0; i < saveSlots.size(); i++)
    {
        if (!saveSlots[i].opened) continue;

        Json json;
        json = saveSlots[i].ToJSON();
        json["version"] = 3;
        json.Save(GetSlotPath(i));
    }
}

void MigrateV0(SaveSlot& slot)
{
    int lastPerlinSeed = perlinSeed;
    perlinSeed = slot.seed;
    for (size_t i = 0; i < slot.islands.size(); i++)
    {
        auto& island = slot.islands[i];
        if (!island.colonized) continue;
        for (int j = 0; j < island.peopleCount; j++)
        {
            slot.people.emplace_back(island.GetRandomPoint(), i);
        }
    }
    perlinSeed = lastPerlinSeed;
}

void MigrateV1(SaveSlot& slot) { slot.mapSize = {300, 300}; }

void MigrateV2()
{
    // Do nothing
    // for (size_t i = 0; i < saveSlots.size(); i++)
    // {
    //     if (saveSlots[i].seed < 0) continue;
    //     LoadFromSlot(i);
    //     GeneratePathCache(saveSlots[i].islands, saveSlots[i].pathCache);
    //     saveSlots[i].ships = {};
    // }
}

void LoadSavesJson()
{
    Json json = Json::Load("saves.json");

    int version = json["version"].GetInt();

    saveSlots.clear();
    for (size_t i = 0; i < json["saves"].size(); i++)
    {
        saveSlots.emplace_back();
        saveSlots.back().LoadJSON(json["saves"][i]);

        if (version == 0)
        {
            MigrateV0(saveSlots[i]);
            version = 1;
        }
        if (version == 1)
        {
            MigrateV1(saveSlots[i]);
            version = 2;
        }
        if (version == 2)
        {
            version = 3;
        }
    }
}

SaveSlot LoadFile(const std::filesystem::path& path)
{
    SaveSlot slot;

    Json json = Json::Load(path);

    int version = json["version"].GetInt();

    slot.LoadJSON(json);

    if (version == 0)
    {
        MigrateV0(saveSlots.back());
        version = 1;
    }
    if (version == 1)
    {
        MigrateV1(saveSlots.back());
        version = 2;
    }
    if (version == 2)
    {
        version = 3;
    }

    return slot;
}

void LoadProgress()
{
    if (std::filesystem::exists("saves.json"))
    {
        LoadSavesJson();
        for (auto& slot: saveSlots)
        {
            slot.opened = true;
        }
        SaveProgress();
        std::filesystem::remove("saves.json");
        return;
    }

    if (!std::filesystem::exists("saves/"))
    {
        SaveProgress();
        return;
    }

    FixSaveIds();
    saveSlots.clear();
    for (auto& file: std::filesystem::directory_iterator("saves/"))
    {
        if (!file.is_regular_file()) continue;

        saveSlots.emplace_back(LoadFile(file));

        // Strip the save slot of heavy data (restored later on demand)
        saveSlots.back().islands.clear();
        saveSlots.back().people.clear();
        saveSlots.back().ships.clear();
    }
    loadMapMenu->ReloadSlots();
}
