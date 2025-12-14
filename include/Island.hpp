// SPDX-FileCopyrightText: 2025 SemkiShow
//
// SPDX-License-Identifier: GPL-3.0-only

#pragma once

#include <raylib.h>
#include <vector>

struct Biome
{
    float startLevel;
    Color color;
    Biome(double startLevel, const Color& color) : startLevel(startLevel), color(color) {}
};

#define TREE_COEFFICIENT 2
#define TREE_GROWTH_COEFFICIENT 0.5
#define IRON_COEFFICIENT 0.5

struct Island
{
    Vector2 p1 = {0, 0}, p2 = {0, 0};
    float area = 0;
    int treeCount = 0, ironCount = 0;
    bool colonized = false;

    Island(Vector2 p1, Vector2 p2, float area, int treeCount, int ironCount)
        : p1(p1), p2(p2), area(area), treeCount(treeCount), ironCount(ironCount)
    {
    }
};

extern std::vector<Biome> biomes;
extern std::vector<Island> islands;
extern std::vector<std::pair<Vector2, Color>> points;

void BuildIslands(float stepSize = 0.1f);
