// SPDX-FileCopyrightText: 2025 SemkiShow
//
// SPDX-License-Identifier: GPL-3.0-only

#include "Progress.hpp"
#include "Drawing.hpp"
#include "Human.hpp"
#include "Island.hpp"
#include "Languages.hpp"
#include "Perlin.hpp"
#include "Settings.hpp"
#include "Ship.hpp"
#include <ctime>
#include <filesystem>

std::vector<SaveSlot> saveSlots(MAX_SAVE_SLOTS);
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
    json["pathCache"].format = JsonFormat::Newline;
    for (auto& path : pathCache) {
        Json pathJson;
        pathJson["key"].format = JsonFormat::Inline;
        pathJson["key"].push_back(path.first.first);
        pathJson["key"].push_back(path.first.second);
        pathJson["paths"].format = JsonFormat::Newline;
        for (auto& p : path.second) {
            Json singlePathJson;
            singlePathJson.format = JsonFormat::Inline;
            for (auto& point : p) {
                Json pointJson;
                pointJson.format = JsonFormat::Inline;
                pointJson.push_back(point.x);
                pointJson.push_back(point.y);
                singlePathJson.push_back(pointJson);
            }
            pathJson["paths"].push_back(singlePathJson);
        }
        json["pathCache"].push_back(pathJson);
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

    json["mapSize"].format = JsonFormat::Inline;
    json["mapSize"].push_back(this->mapSize.x);
    json["mapSize"].push_back(this->mapSize.y);

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
    this->pathCache.clear();
    for (size_t i = 0; i < json["pathCache"].size(); i++) {
        auto& pathJson = json["pathCache"][i];
        std::pair<int, int> key = {pathJson["key"][0].GetInt(), pathJson["key"][1].GetInt()};
        std::vector<Path> paths;
        for (size_t j = 0; j < pathJson["paths"].size(); j++) {
            Path p;
            auto& singlePathJson = pathJson["paths"][j];
            for (size_t k = 0; k < singlePathJson.size(); k++) {
                Vector2 point = {static_cast<float>(singlePathJson[k][0].GetDouble()),
                                 static_cast<float>(singlePathJson[k][1].GetDouble())};
                p.push_back(point);
            }
            paths.push_back(p);
        }
        this->pathCache[key] = paths;
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
    this->mapSize = {static_cast<float>(json["mapSize"][0].GetDouble()),
                     static_cast<float>(json["mapSize"][1].GetDouble())};
}

void SaveToSlot(int idx)
{
    if (idx < 0) return;
    saveSlots[idx].seed = perlinSeed;
    saveSlots[idx].islands = islands;
    saveSlots[idx].pathCache = pathCache;
    saveSlots[idx].ships = ships;
    saveSlots[idx].people = people;
    saveSlots[idx].woodTotal = woodTotal;
    saveSlots[idx].ironTotal = ironTotal;
    saveSlots[idx].peopleTotal = peopleTotal;
    saveSlots[idx].name = labels["Slot"] + " " + std::to_string(idx + 1);
    saveSlots[idx].mapSize = mapSize;
}

void LoadFromSlot(int idx)
{
    currentSlot = idx;
    if (saveSlots[idx].seed == -1)
    {
        BuildMap();
        SaveToSlot(idx);
        return;
    }

    perlinSeed = saveSlots[idx].seed;
    islands = saveSlots[idx].islands;
    pathCache = saveSlots[idx].pathCache;
    ships = saveSlots[idx].ships;
    people = saveSlots[idx].people;
    woodTotal = saveSlots[idx].woodTotal;
    ironTotal = saveSlots[idx].ironTotal;
    peopleTotal = saveSlots[idx].peopleTotal;
    mapSize = saveSlots[idx].mapSize;

    SetShaderValue(perlinShader, GetShaderLocation(perlinShader, "uSeed"), &perlinSeed,
                   SHADER_UNIFORM_INT);
}

void EmptySlot(int idx) { saveSlots[idx] = {}; }

void SaveProgress()
{
    SaveToSlot(currentSlot);

    Json json;

    json["version"] = 3;

    for (size_t i = 0; i < MAX_SAVE_SLOTS; i++)
    {
        json["saves"].push_back(saveSlots[i].ToJSON());
    }

    json.Save("saves.json");
}

void MigrateV0()
{
    int lastPerlinSeed = perlinSeed;
    for (auto& slot: saveSlots)
    {
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
    }
    perlinSeed = lastPerlinSeed;
}

void MigrateV1()
{
    for (auto& slot: saveSlots)
    {
        slot.mapSize = {300, 300};
    }
}

void MigrateV2()
{
    for (size_t i = 0; i < saveSlots.size(); i++)
    {
        if (saveSlots[i].seed < 0) continue;
        LoadFromSlot(i);
        GeneratePathCache(saveSlots[i].islands, saveSlots[i].pathCache);
        saveSlots[i].ships = {};
    }
}

void LoadProgress()
{
    if (!std::filesystem::exists("saves.json"))
    {
        SaveProgress();
        return;
    }

    Json json = Json::Load("saves.json");

    int version = json["version"].GetInt();

    for (size_t i = 0; i < json["saves"].size(); i++)
    {
        saveSlots[i].LoadJSON(json["saves"][i]);
    }

    if (version == 0)
    {
        MigrateV0();
        version = 1;
    }
    if (version == 1)
    {
        MigrateV1();
        version = 2;
    }
    if (version == 2)
    {
        MigrateV2();
        version = 3;
    }
}
