// SPDX-FileCopyrightText: 2026 SemkiShow
//
// SPDX-License-Identifier: GPL-3.0-only

#include "Utils.hpp"
#include "Drawing.hpp"
#include "Json.hpp"
#include <cstdlib>
#include <ostream>
#include <raylib.h>

float GetRandomFloat(float a, float b) { return rand() * 1.0f / RAND_MAX * (b - a) + a; }

void DrawTextCustom(const char* text, Vector2 pos, int fontSize, Color color)
{
    DrawTextEx(myFont, text, pos, fontSize, myFontSpacing, color);
}

std::ostream& operator<<(std::ostream& out, const Vector2& vec)
{
    out << vec.x << ' ' << vec.y;
    return out;
}

Json Vector2ToJson(Vector2 v)
{
    Json json;
    json.format = JsonFormat::Inline;
    json.push_back(v.x);
    json.push_back(v.y);
    return json;
}

Vector2 JsonToVector2(Json& json)
{
    return {static_cast<float>(json[0].GetDouble()), static_cast<float>(json[1].GetDouble())};
}

bool InsideScreen(Vector2 v)
{
    return v.x >= 0 && v.x < windowSize.x && v.y >= 0 && v.y < windowSize.y;
}

std::string FormatTime(double time)
{
    std::stringstream ss;
    ss << (int)time / 3600 << ':' << (int)time / 60 << ':' << (int)time % 60 << '.'
       << int(time * 1000) % 1000;
    return ss.str();
}
