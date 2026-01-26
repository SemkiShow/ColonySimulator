// SPDX-FileCopyrightText: 2025 SemkiShow
//
// SPDX-License-Identifier: GPL-3.0-only

#pragma once

#include <ostream>

struct Vector2;
struct Color;
class Json;

float GetRandomFloat(float a, float b);
void DrawTextCustom(const char* text, Vector2 pos, int fontSize, Color color);
std::ostream& operator<<(std::ostream& out, const Vector2& vec);
Json Vector2ToJson(Vector2 v);
Vector2 JsonToVector2(Json& json);
bool InsideScreen(Vector2 v);
std::string FormatTime(double time);
