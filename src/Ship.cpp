// SPDX-FileCopyrightText: 2025 Jaraslau Zaitsau
//
// SPDX-License-Identifier: GPL-3.0-only

#include "Ship.hpp"
#include "Island.hpp"
#include "Perlin.hpp"
#include <raymath.h>
#include <vector>

Ship::Ship(Island& source, Island& destination) : source(source), destination(destination)
{
    // Find water nearest to the other island
    pos = source.GetRandomPoint();
    Vector2 end = destination.GetRandomPoint();
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

    path = FindPath(pos, end, false);
    nextPointDir = Vector2Normalize(path[0] - pos);
}

void Ship::Move(float deltaTime)
{
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
            // Reached destination
            return;
        }
        nextPointDir = Vector2Normalize(path[nextPointIdx] - pos);
        rotation = atan2f(nextPointDir.y, nextPointDir.x) * 180.0f / PI;
    }
}

std::vector<Ship> ships;
