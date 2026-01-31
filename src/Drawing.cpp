// SPDX-FileCopyrightText: 2025 SemkiShow
// SPDX-FileContributor: Jaraslau Zaitsau
//
// SPDX-License-Identifier: GPL-3.0-only

#include "Drawing.hpp"
#include "Drawing/Game.hpp"
#include "Island.hpp"
#include "Perlin.hpp"
#include "Settings.hpp"
#include "Sound.hpp"
#include "UI.hpp"
#include "UI/EditIsland.hpp"
#include "UI/Game.hpp"
#include "UI/Pause.hpp"
#include "UI/Victory.hpp"
#include <RCore/Conversions.hpp>
#include <ctime>
#include <raylib.h>
#include <raymath.h>

bool shouldClose = false;

Vector2 windowSize{16 * 50, 9 * 50};
double timer = 0;

Shader perlinShader;

Texture lockTexture;
Texture woodTexture;
Texture ironTexture;
Texture humanTexture;
Texture shipTexture;

Font myFont;
std::shared_ptr<RFont> rayuiFont;

Vector2 RaylibToGlsl(Vector2 v)
{
    v.y = windowSize.y - v.y;
    v -= windowSize / 2;
    v *= GetWindowScaleDPI();
    v *= perlinScale;
    v += perlinOffset;
    return v;
}

Vector2 GlslToRaylib(Vector2 v)
{
    v.y *= -1;
    v /= GetWindowScaleDPI() * perlinScale;
    v += windowSize / 2;
    v.x -= perlinOffset.x / perlinScale / GetWindowScaleDPI().x;
    v.y += perlinOffset.y / perlinScale / GetWindowScaleDPI().y;
    return v;
}

void LoadResources()
{
    lockTexture = LoadTexture("resources/textures/lock.png");
    woodTexture = LoadTexture("resources/textures/wooden_log.png");
    ironTexture = LoadTexture("resources/textures/iron_ingot.png");
    humanTexture = LoadTexture("resources/textures/human.png");
    shipTexture = LoadTexture("resources/textures/ship.png");

    const char* symbols =
        "`1234567890-=qwertyuiop[]\\asdfghjkl;'zxcvbnm,./~!@#$%^&*()_+QWERTYUIOP{}|ASDFGHJKL:\"ZXCVBNM<>?ęóąśłżźćńĘÓĄŚŁŻŹĆŃ";
    int codepointCount = 0;
    int* codepoints = LoadCodepoints(symbols, &codepointCount);
    myFont = LoadFontEx("resources/fonts/JetBrainsMono-Bold.ttf", 512, codepoints, codepointCount);

    rayuiFont = std::make_shared<RFont>(rui::FromRaylib(myFont));
    app->SetFont(rayuiFont);

    perlinShader = LoadShader(0, "resources/shaders/Perlin.fs");

    int biomeCount = (int)biomes.size();
    SetShaderValue(perlinShader, GetShaderLocation(perlinShader, "uBiomeCount"), &biomeCount,
                   SHADER_UNIFORM_INT);

    {
        float starts[8];
        for (size_t i = 0; i < biomes.size(); i++) starts[i] = biomes[i].startLevel;

        SetShaderValueV(perlinShader, GetShaderLocation(perlinShader, "uBiomeStart"), starts,
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

        SetShaderValueV(perlinShader, GetShaderLocation(perlinShader, "uBiomeColor"), colors,
                        SHADER_UNIFORM_VEC4, biomeCount);
    }

    LoadSounds();
}

void UpdateWindowSize()
{
    windowSize = {(float)GetRenderWidth(), (float)GetRenderHeight()};
    windowSize /= GetWindowScaleDPI();
}

void DrawFrame()
{
    if (IsWindowMinimized())
    {
        if (gameMenu->IsVisible() && !pauseMenu->IsVisible())
        {
            pauseMenu->SetVisible(true);
        }

        PollInputEvents();
        WaitTime(0.1);
        return;
    }

    if (gameMenu->IsVisible() && !pauseMenu->IsVisible() && !editIslandMenu->IsVisible() &&
        !victoryMenu->IsVisible())
    {
        ProcessPlayerInput();
    }

    BeginDrawing();

    ClearBackground(BLACK);

    UpdateWindowSize();

    app->Update();
    app->Draw();

    if (showFPS) DrawFPS(0, 0);

    EndDrawing();

    UpdateSounds();
}

void FreeResources()
{
    FreeSounds();

    UnloadShader(perlinShader);

    UnloadTexture(lockTexture);
    UnloadTexture(woodTexture);
    UnloadTexture(ironTexture);
    UnloadTexture(humanTexture);
    UnloadTexture(shipTexture);

    UnloadFont(myFont);
}
