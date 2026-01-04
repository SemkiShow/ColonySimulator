// SPDX-FileCopyrightText: 2025 Jaraslau Zaitsau
//
// SPDX-License-Identifier: GPL-3.0-only

#pragma once

#include "Island.hpp"
#include "Pathfinding.hpp"

#define SHIP_SPEED 25.0f

struct Ship
{
    size_t source_index;
    size_t target_index;
    Vector2 pos;
    float rotation = 0;
    Path path;
    Vector2 nextPointDir;
    unsigned int nextPointIdx = 0;
    int people;
    bool reached = false;

    Ship(size_t source_index, size_t target_index, int peopleCount = 1);
    void Move(float deltaTime);
};

extern std::vector<Ship> ships;
