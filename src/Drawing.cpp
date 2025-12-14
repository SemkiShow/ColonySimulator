// SPDX-FileCopyrightText: 2025 SemkiShow
//
// SPDX-License-Identifier: GPL-3.0-only

#include "Drawing.hpp"
#include "Island.hpp"
#include "Perlin.hpp"
#include "Settings.hpp"
#include "UI.hpp"
#include <algorithm>
#include <ctime>
#include <iostream>

Vector2 windowSize{16 * 50, 9 * 50};
double timer = 0;
bool lastVsync = vsync;

Shader biomeShader;
Shader islandShader;

void InitGPU()
{
    biomeShader = LoadShader(0, "resources/Perlin.fs");

    int biomeCount = (int)biomes.size();
    SetShaderValue(biomeShader, GetShaderLocation(biomeShader, "uBiomeCount"), &biomeCount,
                   SHADER_UNIFORM_INT);

    // perlinSeed = GetTime();
    SetShaderValue(biomeShader, GetShaderLocation(biomeShader, "uSeed"), &perlinSeed,
                   SHADER_UNIFORM_FLOAT);

    {
        float starts[8];
        for (size_t i = 0; i < biomes.size(); i++)
            starts[i] = biomes[i].startLevel;

        SetShaderValueV(biomeShader, GetShaderLocation(biomeShader, "uBiomeStart"), starts,
                        SHADER_UNIFORM_FLOAT, biomeCount);
    }

    {
        float colors[8 * 4];
        for (size_t i = 0; i < biomes.size(); i++)
        {
            colors[i * 4 + 0] = biomes[i].color.r / 255.0f;
            colors[i * 4 + 1] = biomes[i].color.g / 255.0f;
            colors[i * 4 + 2] = biomes[i].color.b / 255.0f;
            colors[i * 4 + 3] = 1.0f;
        }

        SetShaderValueV(biomeShader, GetShaderLocation(biomeShader, "uBiomeColor"), colors,
                        SHADER_UNIFORM_VEC4, biomeCount);
    }

    BuildIslands();

    islandShader = LoadShader(0, "resources/Island.fs");

    int islandsCount = islands.size();
    SetShaderValue(islandShader, GetShaderLocation(islandShader, "uIslandsCount"), &islandsCount,
                   SHADER_UNIFORM_INT);

    Vector2 islandStarts[512], islandEnds[512];
    for (size_t i = 0; i < islands.size(); i++)
    {
        islandStarts[i] = islands[i].p1;
        islandEnds[i] = islands[i].p2;
    }
    SetShaderValueV(islandShader, GetShaderLocation(islandShader, "uIslandStarts"),
                    (float*)&islandStarts, SHADER_UNIFORM_VEC2, islandsCount);
    SetShaderValueV(islandShader, GetShaderLocation(islandShader, "uIslandEnds"),
                    (float*)&islandEnds, SHADER_UNIFORM_VEC2, islandsCount);
}

void DrawFrame()
{
    BeginDrawing();

    ClearBackground(BLACK);

    float scale = perlinScale;
    SetShaderValue(biomeShader, GetShaderLocation(biomeShader, "uScale"), &scale,
                   SHADER_UNIFORM_FLOAT);
    SetShaderValue(islandShader, GetShaderLocation(islandShader, "uScale"), &scale,
                   SHADER_UNIFORM_FLOAT);

    windowSize = {(float)GetRenderWidth(), (float)GetRenderHeight()};
    SetShaderValue(biomeShader, GetShaderLocation(biomeShader, "uResolution"), (float*)&windowSize,
                   SHADER_UNIFORM_VEC2);
    SetShaderValue(islandShader, GetShaderLocation(islandShader, "uResolution"),
                   (float*)&windowSize, SHADER_UNIFORM_VEC2);
#if !defined(PLATFORM_WEB)
    windowSize /= GetWindowScaleDPI();
#endif

    SetShaderValue(biomeShader, GetShaderLocation(biomeShader, "uOffset"), (float*)&perlinOffset,
                   SHADER_UNIFORM_VEC2);
    SetShaderValue(islandShader, GetShaderLocation(islandShader, "uOffset"), (float*)&perlinOffset,
                   SHADER_UNIFORM_VEC2);

    SetShaderValue(biomeShader, GetShaderLocation(biomeShader, "uMapSize"), (float*)&mapSize,
                   SHADER_UNIFORM_VEC2);

    Vector2 mousePosition = GetMousePosition();
    mousePosition.y = windowSize.y - mousePosition.y;
    mousePosition -= windowSize / 2;
    mousePosition *= GetWindowScaleDPI();
    mousePosition *= perlinScale;
    mousePosition += perlinOffset;
    SetShaderValue(islandShader, GetShaderLocation(islandShader, "uMouse"), (float*)&mousePosition,
                   SHADER_UNIFORM_VEC2);

    BeginShaderMode(biomeShader);
    DrawRectangle(0, 0, windowSize.x, windowSize.y, WHITE);
    EndShaderMode();

    if (showIslandsBoxes)
    {
        BeginShaderMode(islandShader);
        DrawRectangle(0, 0, windowSize.x, windowSize.y, WHITE);
        EndShaderMode();
    }

    DrawUI();

    EndDrawing();

    double deltaTime = GetTime() - timer;

    if (IsKeyDown(KEY_UP)) perlinOffset.y += panSensitivity * perlinScale * deltaTime;
    if (IsKeyDown(KEY_DOWN)) perlinOffset.y -= panSensitivity * perlinScale * deltaTime;
    if (IsKeyDown(KEY_LEFT)) perlinOffset.x -= panSensitivity * perlinScale * deltaTime;
    if (IsKeyDown(KEY_RIGHT)) perlinOffset.x += panSensitivity * perlinScale * deltaTime;

    double wheelMove = GetMouseWheelMove();
    if (wheelMove > 0) perlinScale -= wheelSensitivity * wheelMove * deltaTime;
    if (wheelMove < 0) perlinScale -= wheelSensitivity * wheelMove * deltaTime;
    perlinScale = std::max(0.0, perlinScale);

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
    {
        std::cout << "Mouse pressed!\n";
        Vector2 v = GetMousePosition();
        v.y = windowSize.y - v.y;
        v -= windowSize / 2;
        v *= GetWindowScaleDPI();
        v *= perlinScale;
        v += perlinOffset;
        for (size_t i = 0; i < islands.size(); i++)
        {
            if (v.x >= islands[i].p1.x && v.x <= islands[i].p2.x && v.y >= islands[i].p1.y &&
                v.y <= islands[i].p2.y)
            {
                std::cout << "Clicked on island with id: " << i << '\n';
            }
            
        }
    }

    timer = GetTime();

    if (lastVsync != vsync)
    {
        lastVsync = vsync;
        if (!vsync)
            ClearWindowState(FLAG_VSYNC_HINT);
        else
            SetWindowState(FLAG_VSYNC_HINT);
    }
}
