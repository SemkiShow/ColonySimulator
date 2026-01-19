// SPDX-FileCopyrightText: 2026 SemkiShow
//
// SPDX-License-Identifier: GPL-3.0-only

#include "Map.hpp"
#include "Human.hpp"
#include "Island.hpp"
#include "Languages.hpp"
#include "Perlin.hpp"
#include "Settings.hpp"
#include "UI/Loading.hpp"
#include <cfloat>
#include <climits>
#include <cmath>
#include <iostream>
#include <raymath.h>
#include <unordered_map>
#include <vector>

#define K_WOOD_COLONIZE 0.05f
#define K_IRON_COLONIZE 0.004f
#define K_WOOD 0.02f
#define K_WOOD_GROWTH 0.002f
#define K_IRON 0.005f
#define K_PEOPLE 0.001f
#define K_PEOPLE_GROWTH 0.001f
#define K_PEOPLE_MAX 0.1f

void BuildIslands(float& loadingPercent, std::atomic<bool>& finished, float stepSize)
{
    // Find islands
    size_t maxX = ceil(mapSize.x / stepSize) + 1, maxY = ceil(mapSize.y / stepSize) + 1;
    std::vector<std::vector<int>> map(maxY, std::vector<int>(maxX, INT_MAX));
    std::unordered_map<int, int> same;
    size_t counter = 0;
    for (size_t i = 0; i < maxY; i++)
    {
        for (size_t j = 0; j < maxX; j++)
        {
            if (GetPerlin({j * stepSize - mapSize.x / 2, i * stepSize - mapSize.y / 2}) <
                LAND_START)
                continue;
            if (j > 0) map[i][j] = fmin(map[i][j], map[i][j - 1]);
            if (i > 0) map[i][j] = fmin(map[i][j], map[i - 1][j]);
            if (j > 0 && i > 0)
            {
                auto n1 = map[i][j - 1], n2 = map[i - 1][j];
                if (n1 > n2) std::swap(n1, n2);
                if (n1 != n2 && same.find(n2) == same.end()) same[n2] = n1;
            }
            if (map[i][j] == INT_MAX) map[i][j] = counter++;
        }
        loadingPercent += 1.0f / maxY / 2 * 100;
    }
    std::cout << "Total island count: " << counter - same.size() << '\n';

    // Calculate islands' areas
    std::vector<int> islandAreas(counter, 0);
    std::vector<std::pair<Vector2, Vector2>> islandCorners(
        counter, {{FLT_MAX, FLT_MAX}, {-FLT_MAX, -FLT_MAX}});
    for (size_t i = 0; i < maxY; i++)
    {
        for (size_t j = 0; j < maxX; j++)
        {
            if (map[i][j] == INT_MAX) continue;

            int idx = map[i][j];
            while (same.find(idx) != same.end())
                idx = same[idx];
            map[i][j] = idx;
            islandAreas[idx]++;

            auto& corner = islandCorners[idx];
            corner.first.x = fmin(corner.first.x, j * stepSize - mapSize.x / 2);
            corner.first.y = fmin(corner.first.y, i * stepSize - mapSize.y / 2);
            corner.second.x = fmax(corner.second.x, j * stepSize - mapSize.x / 2);
            corner.second.y = fmax(corner.second.y, i * stepSize - mapSize.y / 2);
        }
        loadingPercent += 1.0f / maxY / 2 * 100;
    }

    // Add large enough islands to the main vector
    int minIslandArea = 125 / stepSize / stepSize;
    int passed = 0;
    islands.clear();
    for (size_t i = 0; i < counter; i++)
    {
        if (islandAreas[i] < minIslandArea) continue;

        auto& corner = islandCorners[i];
        Vector2 center = {(corner.second.x + corner.first.x) / 2,
                          (corner.second.y + corner.first.y) / 2};
        float distance = Vector2Distance(center, {0, 0});
        float area = islandAreas[i] * stepSize * stepSize;
        float cost = distance * area;
        islands.emplace_back(corner.first, corner.second, area, cost * K_WOOD_COLONIZE,
                             cost * K_IRON_COLONIZE, cost * K_WOOD, cost * K_WOOD_GROWTH,
                             cost * K_IRON, area * K_PEOPLE_GROWTH, area * K_PEOPLE_MAX);
        islands.back().index = islands.size() - 1;
        passed++;
    }
    std::cout << "Found " << passed << " large enough islands\n";

    // Set the closest island to center as colonized
    int minDistanceIslandIdx = 0;
    for (size_t i = 0; i < islands.size(); i++)
    {
        const auto &island = islands[i], &minIsland = islands[minDistanceIslandIdx];
        if (abs(Vector2Distance(Vector2{0, 0}, (island.p1 + island.p2) / 2)) <
            abs(Vector2Distance(Vector2{0, 0}, (minIsland.p1 + minIsland.p2) / 2)))
            minDistanceIslandIdx = i;
    }
    islands[minDistanceIslandIdx].colonized = true;

    // Set start resources
    auto& startIsland = islands[minDistanceIslandIdx];
    peopleTotal = startIsland.area * K_PEOPLE;
    peopleTotal = fmax(2, peopleTotal);
    startIsland.peopleCount = peopleTotal;
    for (int i = 0; i < startIsland.peopleCount; i++)
    {
        people.emplace_back(startIsland.GetRandomPoint(), minDistanceIslandIdx);
    }

    // Prevent softlocking by having enough people to extract iron and enough iron to colonize
    startIsland.peopleMax = fmax(3, startIsland.peopleMax);
    startIsland.ironCount *= 10;

    finished = true;
}

void BuildMap()
{
    auto func = [](std::string& label, float& loadingPercent, std::atomic<bool>& finished)
    {
        label = _("Loading map...");
        woodTotal = ironTotal = peopleTotal = 0;
        BuildIslands(loadingPercent, finished, 0.1f);
    };
    ShowLoadingScreen(true, func);
}
