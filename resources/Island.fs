// SPDX-FileCopyrightText: 2025 SemkiShow
//
// SPDX-License-Identifier: GPL-3.0-only

#version 330

uniform float uScale;
uniform vec2 uResolution;
uniform vec2 uOffset;

uniform vec2 uMouse;

uniform vec2 uIslandStarts[512];
uniform vec2 uIslandEnds[512];
uniform int uIslandsCount;

bool inside(vec2 v, int i)
{
    return v.x >= uIslandStarts[i].x && v.x <= uIslandEnds[i].x && v.y >= uIslandStarts[i].y &&
           v.y <= uIslandEnds[i].y;
}

vec4 drawIslands(vec2 uv)
{
    bool found = false;
    for (int i = 0; i < uIslandsCount; i++)
    {
        if (inside(uv, i) && inside(uMouse, i))
        {
            found = true;
            break;
        }
    }
    if (found) return vec4(1, 0, 0, 0.5);
    return vec4(0, 0, 0, 0);
}

out vec4 fragColor;

void main()
{
    vec2 uv = (gl_FragCoord.xy - uResolution / 2) * uScale + uOffset;

    fragColor = drawIslands(uv);
}
