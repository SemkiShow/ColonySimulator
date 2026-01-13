// SPDX-FileCopyrightText: 2025 SemkiShow
//
// SPDX-License-Identifier: GPL-3.0-only

#version 330

uniform float uScale;
uniform vec2 uResolution;
uniform vec2 uOffset;

uniform int uSeed;
uniform vec2 uMapSize;

uniform float uBiomeStart[8];
uniform vec4 uBiomeColor[8];
uniform int uBiomeCount;

const vec2 g[8] = vec2[](vec2(1, 0), vec2(-1, 0), vec2(0, 1), vec2(0, -1), vec2(0.707, 0.707),
                         vec2(-0.707, 0.707), vec2(0.707, -0.707), vec2(-0.707, -0.707));

float fade(float t) { return t * t * t * (t * (t * 6 - 15) + 10); }

int hash(int x, int y)
{
    uint h = uint(uSeed) ^ (uint(x) * 0x45d9f3bU) ^ (uint(y) * 0x9e3779b9U);
    h = ((h >> 16) ^ h) * 0x45d9f3bU;
    h = ((h >> 16) ^ h) * 0x45d9f3bU;
    h = (h >> 16) ^ h;
    return int(h);
}

float perlin(vec2 pos)
{
    vec2 i = floor(pos);
    vec2 f = fract(pos);

    int X = int(i.x);
    int Y = int(i.y);

    int aa = hash(X, Y);
    int ab = hash(X, Y + 1);
    int ba = hash(X + 1, Y);
    int bb = hash(X + 1, Y + 1);

    float n00 = dot(g[aa & 7], f);
    float n01 = dot(g[ab & 7], f - vec2(0.0, 1.0));
    float n10 = dot(g[ba & 7], f - vec2(1.0, 0.0));
    float n11 = dot(g[bb & 7], f - vec2(1.0, 1.0));

    vec2 u = vec2(fade(f.x), fade(f.y));
    return mix(mix(n00, n10, u.x), mix(n01, n11, u.x), u.y);
}

float getPerlin(vec2 v)
{
    return (0.3 * perlin(v) + 2 * perlin(0.1 * v) + 3.5 * perlin(0.05 * v)) / 4.20;
}

vec4 applyBiomes(float v)
{
    vec4 color = uBiomeColor[0];

    for (int i = 1; i < uBiomeCount; i++)
    {
        if (v >= uBiomeStart[i])
        {
            float t = (v - uBiomeStart[i - 1]) / (uBiomeStart[i] - uBiomeStart[i - 1]);
            color = mix(uBiomeColor[i - 1], uBiomeColor[i], t);
        }
    }

    return color;
}

out vec4 fragColor;

void main()
{
    vec2 uv = (gl_FragCoord.xy - uResolution / 2) * uScale + uOffset;

    if (uv.x < -uMapSize.x / 2 || uv.x >= uMapSize.x / 2 || uv.y < -uMapSize.y / 2 ||
        uv.y >= uMapSize.y / 2)
    {
        fragColor = vec4(0, 0, 0, 1);
        return;
    }

    float n = getPerlin(uv);
    fragColor = applyBiomes(n);
}
