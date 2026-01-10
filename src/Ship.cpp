// SPDX-FileCopyrightText: 2025 Jaraslau Zaitsau
// SPDX-FileContributor: SemkiShow
//
// SPDX-License-Identifier: GPL-3.0-only

#include "Ship.hpp"
#include "Island.hpp"
#include "Pathfinding.hpp"
#include "Perlin.hpp"
#include <raymath.h>
#include <vector>

std::vector<Ship> ships;

Ship::Ship(int sourceIndex, int targetIndex, int peopleCount)
    : sourceIndex(sourceIndex), targetIndex(targetIndex), people(peopleCount)
{
    path = pathCache[{sourceIndex, targetIndex}][rand() % PORTS_PER_ISLAND];
    pos = path[0];

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
            reached = true;
            return;
        }

        nextPointDir = Vector2Normalize(path[nextPointIdx] - pos);
        if (nextPointDir.x > 0.1) flip = 1;
        if (nextPointDir.x < -0.1) flip = -1;
    }
}
