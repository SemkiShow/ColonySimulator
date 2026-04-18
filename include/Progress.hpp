// SPDX-FileCopyrightText: 2025 SemkiShow
//
// SPDX-License-Identifier: GPL-3.0-only

#pragma once

#include "Human.hpp"
#include "Island.hpp"
#include "Ship.hpp"
#include <JsonFormat.hpp>
#include <vector>

struct SaveSlot
{
    int seed = -1;
    std::string name = "Empty slot";
    std::vector<Island> islands;
    std::vector<Human> people;
    std::vector<Ship> ships;
    int woodTotal = 0, ironTotal = 0, peopleTotal = 0;
    Vector2 mapSize{300, 300};
    bool completed = false;
    bool deleteLater = false, opened = false;
    double time = 0;

    Json ToJson();
    void LoadJson(Json& json);
};

extern std::vector<SaveSlot> saveSlots;
extern int currentSlot;

bool IsSlotValid(int idx);
void SaveToSlot(int idx);
void LoadFromSlot(int idx, bool generatePathMap);
void FixSaveIds();
std::string GetSlotPath(int idx);
void SaveProgress();
SaveSlot LoadFile(const std::filesystem::path& path);
void LoadProgress();
