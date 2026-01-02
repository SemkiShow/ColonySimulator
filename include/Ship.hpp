// SPDX-FileCopyrightText: 2025 Jaraslau Zaitsau
//
// SPDX-License-Identifier: GPL-3.0-only

#pragma once

#include "Island.hpp"
#include "Pathfinding.hpp"

#define SHIP_SPEED 0.05f

struct Ship
{
    Island& source;
    Island& destination;
    Vector2 pos;
    float rotation = 0;
    Path path;
    Vector2 nextPointDir;
    unsigned int nextPointIdx = 0;

    Ship(Island& source, Island& destination);
    void Move(float deltaTime);
};

extern std::vector<Ship> ships;
