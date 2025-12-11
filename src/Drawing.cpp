// SPDX-FileCopyrightText: 2025 SemkiShow
//
// SPDX-License-Identifier: GPL-3.0-only

#include "Drawing.hpp"
#include "Settings.hpp"
#include "UI.hpp"
#include <algorithm>
#include <ctime>
#include <vector>

inline Color rgb(unsigned char r, unsigned char g, unsigned char b) { return {r, g, b, 255}; }

struct Biome
{
    float startLevel;
    Color color;
    Biome(double startLevel, const Color& color) : startLevel(startLevel), color(color) {}
};

std::vector<Biome> biomes = {{-1, rgb(0, 0, 255)},     {-0.5, rgb(0, 136, 255)},
                             {0, rgb(97, 218, 255)},   {0.1, rgb(251, 254, 145)},
                             {0.2, rgb(33, 171, 42)},  {0.5, rgb(184, 184, 205)},
                             {0.6, rgb(255, 255, 255)}};

Vector2 windowSize{16 * 50 * 2, 9 * 50 * 2};
Vector2 perlinOffset = {0, 0};
double timer = 0;
Shader biomeShader;
bool lastVsync = vsync;

void InitGPU()
{
    biomeShader = LoadShader(0, "resources/Perlin.fs");

    int biomeCount = (int)biomes.size();
    SetShaderValue(biomeShader, GetShaderLocation(biomeShader, "uBiomeCount"), &biomeCount,
                   SHADER_UNIFORM_INT);

    float seed = GetTime();
    SetShaderValue(biomeShader, GetShaderLocation(biomeShader, "uSeed"), &seed,
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
}

void DrawFrame()
{
    ClearBackground(BLACK);

    float scale = perlinScale;
    SetShaderValue(biomeShader, GetShaderLocation(biomeShader, "uScale"), &scale,
                   SHADER_UNIFORM_FLOAT);

    windowSize = {(float)GetRenderWidth(), (float)GetRenderHeight()};
    {
        float vec[2] = {windowSize.x, windowSize.y};
        SetShaderValue(biomeShader, GetShaderLocation(biomeShader, "uResolution"), vec,
                       SHADER_UNIFORM_VEC2);
    }
#if !defined(PLATFORM_WEB)
    windowSize.x /= GetWindowScaleDPI().x;
    windowSize.y /= GetWindowScaleDPI().y;
#endif

    {
        float vec[2] = {perlinOffset.x, perlinOffset.y};
        SetShaderValue(biomeShader, GetShaderLocation(biomeShader, "uOffset"), &vec,
                       SHADER_UNIFORM_VEC2);
    }

    {
        float vec[2] = {mapSize.x, mapSize.y};
        SetShaderValue(biomeShader, GetShaderLocation(biomeShader, "uMapSize"), &vec,
                       SHADER_UNIFORM_VEC2);
    }

    BeginShaderMode(biomeShader);
    DrawRectangle(0, 0, windowSize.x, windowSize.y, WHITE);
    EndShaderMode();

    DrawUI();

    double deltaTime = GetTime() - timer;

    if (IsKeyDown(KEY_UP)) perlinOffset.y += panSensitivity * perlinScale * deltaTime;
    if (IsKeyDown(KEY_DOWN)) perlinOffset.y -= panSensitivity * perlinScale * deltaTime;
    if (IsKeyDown(KEY_LEFT)) perlinOffset.x -= panSensitivity * perlinScale * deltaTime;
    if (IsKeyDown(KEY_RIGHT)) perlinOffset.x += panSensitivity * perlinScale * deltaTime;

    double wheelMove = GetMouseWheelMove();
    if (wheelMove > 0) perlinScale -= wheelSensitivity * wheelMove * deltaTime;
    if (wheelMove < 0) perlinScale -= wheelSensitivity * wheelMove * deltaTime;
    perlinScale = std::max(0.0, perlinScale);

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
