// SPDX-FileCopyrightText: 2025 SemkiShow
//
// SPDX-License-Identifier: GPL-3.0-only

#include "Drawing.hpp"
#include "Island.hpp"
#include "Settings.hpp"
#include <raygui.h>
#include <thread>

#if defined(PLATFORM_WEB)
#include <emscripten/emscripten.h>
#endif

int main()
{
    Load();

#if !defined(PLATFORM_WEB)
    int flags = 0;
    if (vsync) flags |= FLAG_VSYNC_HINT;
    flags |= FLAG_WINDOW_HIGHDPI;
    flags |= FLAG_WINDOW_RESIZABLE;
    SetConfigFlags(flags);
#endif
    GuiSetStyle(DEFAULT, TEXT_SIZE, 24);

    InitWindow(windowSize.x, windowSize.y, "Colony Simulator");
    SetExitKey(-1);

    GuiSetFont(GetFontDefault());

    std::atomic<bool> finished(false);
    std::thread initThread(BuildIslands, std::ref(finished), 0.1f);
    initThread.detach();

    while (!finished)
    {
        BeginDrawing();

        ClearBackground(BLACK);

        DrawText("Loading map...", 0, GetRenderHeight() / GetWindowScaleDPI().y - 24, 24, WHITE);

        EndDrawing();
    }

    InitGPU();

#if defined(PLATFORM_WEB)
    emscripten_set_main_loop(DrawFrame, 0, 1);
#else
    while (!WindowShouldClose())
    {
        DrawFrame();
    }
#endif

    Save();
    CloseWindow();

    return 0;
}
