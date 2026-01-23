// SPDX-FileCopyrightText: 2025 SemkiShow
// SPDX-FileContributor: Jaraslau Zaitsau
//
// SPDX-License-Identifier: GPL-3.0-only

#pragma once

#include <memory>

struct Vector2;
struct Shader;
struct Texture;
struct Font;
struct RFont;
struct Sound;

extern bool shouldClose;
extern Vector2 windowSize;

extern Shader perlinShader;

extern Texture lockTexture;
extern Texture woodTexture;
extern Texture ironTexture;
extern Texture humanTexture;
extern Texture shipTexture;

extern Font myFont;
const float myFontSpacing = 1;
extern std::shared_ptr<RFont> rayuiFont;

extern Sound victorySound;

Vector2 RaylibToGlsl(Vector2 v);
Vector2 GlslToRaylib(Vector2 v);

void UpdateWindowSize();
void InitGPU();
void DrawFrame();
void FreeResources();
