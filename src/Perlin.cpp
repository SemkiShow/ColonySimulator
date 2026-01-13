// SPDX-FileCopyrightText: 2025 SemkiShow
//
// SPDX-License-Identifier: GPL-3.0-only

#include "Perlin.hpp"
#include "Settings.hpp"
#include <cmath>
#include <raymath.h>

int perlinSeed = 0;
float perlinScale = 0.12f;
Vector2 perlinOffset = {0, 0};

// Fixed gradient vectors (8 directions)
static const Vector2 g[] = {{1, 0},
                            {-1, 0},
                            {0, 1},
                            {0, -1},
                            {0.707f, 0.707f},
                            {-0.707f, 0.707f},
                            {0.707f, -0.707f},
                            {-0.707f, -0.707f}};

float Fade(float t) { return t * t * t * (t * (t * 6.0f - 15.0f) + 10.0f); }

int Hash(int x, int y)
{
    unsigned int h = (unsigned int)perlinSeed;
    h ^= (unsigned int)x * 0x45d9f3b;
    h ^= (unsigned int)y * 0x9e3779b9;
    h = ((h >> 16) ^ h) * 0x45d9f3b;
    h = ((h >> 16) ^ h) * 0x45d9f3b;
    h = (h >> 16) ^ h;
    return (int)h;
}

float Perlin(Vector2 pos)
{
    int X = (int)floorf(pos.x);
    int Y = (int)floorf(pos.y);

    float x = pos.x - floorf(pos.x);
    float y = pos.y - floorf(pos.y);

    float u = Fade(x);
    float v = Fade(y);

    // Hash coordinates to find 4 corners
    int aa = Hash(X, Y);
    int ab = Hash(X, Y + 1);
    int ba = Hash(X + 1, Y);
    int bb = Hash(X + 1, Y + 1);

    // Dot products with fixed gradients
    float n00 = Vector2DotProduct(g[aa & 7], {x, y});
    float n01 = Vector2DotProduct(g[ab & 7], {x, y - 1.0f});
    float n10 = Vector2DotProduct(g[ba & 7], {x - 1.0f, y});
    float n11 = Vector2DotProduct(g[bb & 7], {x - 1.0f, y - 1.0f});

    return Lerp(Lerp(n00, n10, u), Lerp(n01, n11, u), v);
}

float GetPerlin(Vector2 v)
{
    return (0.3f * Perlin(v) + 2.0f * Perlin(Vector2Scale(v, 0.1f)) +
            3.5f * Perlin(Vector2Scale(v, 0.05f))) /
           4.20f;
}

bool InsideMap(Vector2 pos)
{
    return pos.x > -mapSize.x / 2 && pos.x < mapSize.x / 2 && pos.y > -mapSize.y / 2 &&
           pos.y < mapSize.y / 2;
}
