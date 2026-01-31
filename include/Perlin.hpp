// SPDX-FileCopyrightText: 2025 SemkiShow
//
// SPDX-License-Identifier: GPL-3.0-only

#pragma once

struct Vector2;

#define DEFAULT_PERLIN_SCALE 0.12f

float GetPerlin(Vector2 v);
bool InsideMap(Vector2 pos);

extern int perlinSeed;
extern float perlinScale;
extern Vector2 perlinOffset;
