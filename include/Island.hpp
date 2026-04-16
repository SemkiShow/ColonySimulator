// SPDX-FileCopyrightText: 2025 SemkiShow
// SPDX-FileContributor: Jaraslau Zaitsau
//
// SPDX-License-Identifier: GPL-3.0-only

#pragma once

#include "JsonFormat.hpp"
#include <RWidgets/RIconButton.hpp>
#include <raylib.h>
#include <vector>

struct Biome
{
    float startLevel;
    Color color;
    Biome(float startLevel, const Color& color) : startLevel(startLevel), color(color) {}
};

#define GROWTH_PERIOD 1
#define DEFAULT_TAXES 60

struct Island
{
    Vector2 p1 = {0, 0}, p2 = {0, 0};
    std::vector<Vector2> borderPoints;
    float area = 0;
    int woodColonize = 0, ironColonize = 0, woodCount = 0, woodGrowth = 0, woodMax = 0,
        ironCount = 0, peopleCount = 0, peopleMax = 0, futurePeopleCount = 0;
    float peopleGrowth = 0, addPeopleFraction = 0;
    bool colonizationInProgress = false;
    bool colonized = false;
    int taxes = DEFAULT_TAXES, efficiency = 50;
    int index = -1;
    std::shared_ptr<RIconButton> editButton;

    Island() = default;
    Island(Vector2 p1, Vector2 p2, const std::vector<Vector2>& borderPoints, float area,
           int woodColonize, int ironColonize, int woodCount, int woodGrowth, int ironCount,
           float peopleGrowth, int peopleMax)
        : p1(p1), p2(p2), borderPoints(borderPoints), area(area), woodColonize(woodColonize),
          ironColonize(ironColonize), woodCount(woodCount), woodGrowth(woodGrowth),
          ironCount(ironCount), peopleMax(peopleMax), peopleGrowth(peopleGrowth)
    {
        woodMax = woodCount;
    }

    Vector2 GetRandomPoint();
    bool IsPointInside(const Vector2& point);
    void Colonize();
    void SendPeople(int count);
    void AddPeople(int count);
    void GrowthTick();
    void DrawStats();

    Json ToJson();
    static Island LoadJson(Json& json);
};

extern std::vector<Biome> biomes;
extern std::vector<Island> islands;

#define LAND_START biomes[3].startLevel

extern int woodTotal;
extern int ironTotal;
extern int peopleTotal;
