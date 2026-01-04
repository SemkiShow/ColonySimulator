// SPDX-FileCopyrightText: 2025 Jaraslau Zaitsau
//
// SPDX-License-Identifier: GPL-3.0-only

#include "Ship.hpp"
#include "Island.hpp"
#include "Pathfinding.hpp"
#include "Perlin.hpp"
#include <raymath.h>
#include <vector>

Ship::Ship(size_t source_index, size_t target_index, int peopleCount)
    : source_index(source_index), target_index(target_index), people(peopleCount)
{
    // Find water nearest to the other island
    pos = islands[source_index].GetRandomPoint();
    Vector2 end = islands[target_index].GetRandomPoint();
    Vector2 increment = Vector2Normalize(end - pos);

    while (true)
    {
        if (GetPerlin(pos) < LAND_START)
        {
            break;
        }
        pos += increment;
    }
    while (true)
    {
        if (GetPerlin(end) < LAND_START)
        {
            break;
        }
        end -= increment;
    }

    // if (pathCache[source_index][target_index].size() > 0)
    // {
    //     path = pathCache[source_index][target_index];
    // }
    // else
    // {
    //     path = FindPath(pos, end, false, 1.0f);
    //     pathCache[source_index][target_index] = path;
    // } Leave this for now, as path cache needs to be loaded
    path = FindPath(pos, end, false, 1.0f);
    nextPointDir = Vector2Normalize(path[0] - pos);
}

void Ship::Move(float deltaTime)
{
    if (reached) return;
    Vector2 nextPos = pos + nextPointDir * SHIP_SPEED * deltaTime;
    if (Vector2Distance(pos, path[nextPointIdx]) > Vector2Distance(nextPos, path[nextPointIdx]))
    {
        pos = nextPos;
    }
    else
    {
        nextPointIdx++;
        if (nextPointIdx >= path.size())
        {
            islands[target_index].AddPeople(people);
            reached = true;
            return;
        }
        nextPointDir = Vector2Normalize(path[nextPointIdx] - pos);
        rotation = atan2f(nextPointDir.y, nextPointDir.x) * 180.0f / PI;
    }
}

std::vector<Ship> ships;
