// SPDX-FileCopyrightText: 2026 SemkiShow
//
// SPDX-License-Identifier: GPL-3.0-only

#include "Map.hpp"
#include "Human.hpp"
#include "Island.hpp"
#include "Languages.hpp"
#include "Perlin.hpp"
#include "Settings.hpp"
#include "Ship.hpp"
#include "UI/Loading.hpp"
#include <cfloat>
#include <climits>
#include <cmath>
#include <iostream>
#include <raymath.h>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#define K_WOOD_COLONIZE 0.05f
#define K_IRON_COLONIZE 0.004f
#define K_WOOD 0.02f
#define K_WOOD_GROWTH 0.002f
#define K_IRON 0.005f
#define K_PEOPLE 0.001f
#define K_PEOPLE_GROWTH 0.001f
#define K_PEOPLE_MAX 0.1f

template <class T1, class T2>
struct std::hash<std::pair<T1, T2>>
{
    std::size_t operator()(const std::pair<T1, T2>& p) const
    {
        auto h1 = std::hash<T1>{}(p.first);
        auto h2 = std::hash<T2>{}(p.second);
        return h1 ^ (h2 + 0x9e3779b9 + (h1 << 6) + (h1 >> 2));
    }
};

std::vector<Vector2> GetBorderPoints(const std::vector<std::pair<int, int>>& islandPoints,
                                     float stepSize, float approxStepSize)
{
    using Point = std::pair<int, int>;
    if (islandPoints.empty()) return {};

    std::unordered_set<Point> pointSet(islandPoints.begin(), islandPoints.end());

    // Find the start point
    Point startPoint = islandPoints[0];
    for (const auto& p: islandPoints)
    {
        startPoint = std::min(startPoint, p);
    }

    Point dirs[8] = {{0, -1}, {1, -1}, {1, 0}, {1, 1}, {0, 1}, {-1, 1}, {-1, 0}, {-1, -1}};

    // Extract border points using a wall follower algorithm
    std::vector<Vector2> borderPoints;
    Point currentPoint = startPoint;
    int backtrackDir = 7;
    do
    {
        borderPoints.push_back({currentPoint.first * stepSize - mapSize.x / 2,
                                currentPoint.second * stepSize - mapSize.y / 2});

        bool foundNext = false;
        for (int i = 0; i < 8; i++)
        {
            int checkIdx = (backtrackDir + i) % 8;
            Point neighbor = {currentPoint.first + dirs[checkIdx].first,
                              currentPoint.second + dirs[checkIdx].second};

            if (pointSet.count(neighbor))
            {
                currentPoint = neighbor;
                backtrackDir = (checkIdx + 5) % 8;
                foundNext = true;
                break;
            }
        }

        if (!foundNext) break;
    } while (currentPoint != startPoint);

    // Return early if there are not enough points anyway
    if (borderPoints.size() < 3) return borderPoints;

    // Approximate border points as an optimisation
    std::vector<Vector2> approxBorderPoints;
    approxBorderPoints.push_back(borderPoints[0]);
    const float sqrApprox = approxStepSize * approxStepSize;
    for (size_t i = 1; i < borderPoints.size(); i++)
    {
        if (Vector2DistanceSqr(borderPoints[i], approxBorderPoints.back()) >= sqrApprox)
        {
            approxBorderPoints.push_back(borderPoints[i]);
        }
    }

    // Return the original border points if the approximation removed too many points
    return (approxBorderPoints.size() < 3 ? borderPoints : approxBorderPoints);
}

void BuildIslands(float& loadingPercent, std::atomic<bool>& finished, float stepSize)
{
    const float stepsTotal = 3;

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

            int left = (j > 0) ? map[i][j - 1] : INT_MAX;
            int top = (i > 0) ? map[i - 1][j] : INT_MAX;

            // Determine label based on neighbors
            if (left == INT_MAX && top == INT_MAX)
            {
                map[i][j] = counter++;
            }
            else
            {
                // Inherit the smallest label found
                map[i][j] = std::min(left, top);

                // Merge islands if we touch two different valid labels
                if (left != INT_MAX && top != INT_MAX && left != top)
                {
                    int rootLeft = left;
                    while (same.count(rootLeft)) rootLeft = same[rootLeft];

                    int rootTop = top;
                    while (same.count(rootTop)) rootTop = same[rootTop];

                    // If roots are different, merge the larger root into the smaller root
                    if (rootLeft != rootTop)
                    {
                        if (rootLeft < rootTop)
                            same[rootTop] = rootLeft;
                        else
                            same[rootLeft] = rootTop;
                    }
                }
            }
        }
        loadingPercent += 1.0f / maxY / stepsTotal * 100;
    }
    std::cout << "Total island count: " << counter - same.size() << '\n';

    // Calculate islands' areas
    std::vector<int> islandAreas(counter, 0);
    std::vector<std::vector<std::pair<int, int>>> islandPoints(counter);
    std::vector<std::pair<Vector2, Vector2>> islandCorners(
        counter, {{FLT_MAX, FLT_MAX}, {-FLT_MAX, -FLT_MAX}});
    for (size_t i = 0; i < maxY; i++)
    {
        for (size_t j = 0; j < maxX; j++)
        {
            if (map[i][j] == INT_MAX) continue;

            int idx = map[i][j];
            while (same.count(idx)) idx = same[idx];
            map[i][j] = idx;
            islandAreas[idx]++;
            islandPoints[idx].emplace_back(j, i);

            auto& corner = islandCorners[idx];
            corner.first.x = fmin(corner.first.x, j * stepSize - mapSize.x / 2);
            corner.first.y = fmin(corner.first.y, i * stepSize - mapSize.y / 2);
            corner.second.x = fmax(corner.second.x, j * stepSize - mapSize.x / 2);
            corner.second.y = fmax(corner.second.y, i * stepSize - mapSize.y / 2);
        }
        loadingPercent += 1.0f / maxY / stepsTotal * 100;
    }

    // Add large enough islands to the main vector
    int minIslandArea = 125 / stepSize / stepSize;
    int passed = 0;
    for (size_t i = 0; i < counter; i++)
    {
        loadingPercent += 1.0f / counter / stepsTotal * 100;

        if (islandAreas[i] < minIslandArea) continue;

        auto& corner = islandCorners[i];
        auto borderPoints = GetBorderPoints(islandPoints[i], stepSize);
        Vector2 center = {(corner.second.x + corner.first.x) / 2,
                          (corner.second.y + corner.first.y) / 2};
        float distance = Vector2Distance(center, {0, 0});
        float area = islandAreas[i] * stepSize * stepSize;
        float cost = distance * area;
        islands.emplace_back(corner.first, corner.second, borderPoints, area,
                             cost * K_WOOD_COLONIZE, cost * K_IRON_COLONIZE, cost * K_WOOD,
                             cost * K_WOOD_GROWTH, cost * K_IRON, area * K_PEOPLE_GROWTH,
                             area * K_PEOPLE_MAX);
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

    // Set start resources
    auto& startIsland = islands[minDistanceIslandIdx];
    peopleTotal = startIsland.area * K_PEOPLE;
    peopleTotal = fmax(2, peopleTotal);
    startIsland.peopleCount = peopleTotal;
    startIsland.AddPeople(startIsland.peopleCount);

    // Prevent softlocking by having enough people to extract iron and enough iron to colonize
    startIsland.peopleMax = fmax(3, startIsland.peopleMax);
    startIsland.ironCount *= 10;

    finished = true;
}

void ResetMapVariables()
{
    woodTotal = ironTotal = peopleTotal = 0;
    perlinScale = DEFAULT_PERLIN_SCALE;
    perlinOffset = {0, 0};
    islands.clear();
    people.clear();
    ships.clear();
}

void BuildMap()
{
    auto func = [](std::string& label, float& loadingPercent, std::atomic<bool>& finished)
    {
        label = _("Loading map...");
        ResetMapVariables();
        BuildIslands(loadingPercent, finished, 0.1f);
    };
    ShowLoadingScreen(true, func);
}
