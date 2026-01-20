// SPDX-FileCopyrightText: 2026 SemkiShow
//
// SPDX-License-Identifier: GPL-3.0-only

#include "Debug.hpp"
#include "Drawing.hpp"
#include "Island.hpp"
#include "Ship.hpp"
#include <raylib.h>
#include <raymath.h>

void DrawShipPaths()
{
    for (auto& ship: ships)
    {
        Vector2 lastPoint;
        if (!ship.path.empty()) lastPoint = ship.path[0];
        int counter = 0;
        for (auto& point: ship.path)
        {
            DrawLineEx(GlslToRaylib(lastPoint), GlslToRaylib(point), 3,
                       ColorLerp(RED, BLUE, counter * 1.0f / ship.path.size()));
            lastPoint = point;
            counter++;
        }
    }
}

void DrawIslandBounds()
{
    for (auto& island: islands)
    {
        Vector2 p1 = GlslToRaylib(island.p1), p2 = GlslToRaylib(island.p2);
        DrawRectangleV(p1, p2 - p1, {255, 0, 0, 127});
    }
}

void DrawIslandBorderPoints()
{
    const float nodeRadius = 2, lineThickness = 1;

    for (auto& island: islands)
    {
        if (island.borderPoints.empty()) continue;

        Vector2 lastPoint = GlslToRaylib(island.borderPoints[0]);
        for (auto& point: island.borderPoints)
        {
            Vector2 p = GlslToRaylib(point);
            DrawLineEx(lastPoint, p, lineThickness, BLACK);
            DrawCircleV(p, nodeRadius, {255, 0, 0, 127});
            lastPoint = GlslToRaylib(point);
        }
        DrawLineEx(GlslToRaylib(island.borderPoints[0]), lastPoint, lineThickness, BLACK);
    }
}
