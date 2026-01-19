// SPDX-FileCopyrightText: 2025 SemkiShow
//
// SPDX-License-Identifier: GPL-3.0-only

#pragma once

#include "Drawing.hpp"
#include <cstdlib>
#include <ostream>
#include <raylib.h>

inline float GetRandomFloat(float a, float b) { return rand() * 1.0f / RAND_MAX * (b - a) + a; }

inline void DrawTextCustom(const char* text, Vector2 pos, int fontSize, Color color)
{
    DrawTextEx(myFont, text, pos, fontSize, myFontSpacing, color);
}

inline std::ostream& operator<<(std::ostream& out, const Vector2& vec)
{
    out << vec.x << ' ' << vec.y;
    return out;
}
