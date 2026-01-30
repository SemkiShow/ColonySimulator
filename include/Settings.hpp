// SPDX-FileCopyrightText: 2025 SemkiShow
//
// SPDX-License-Identifier: GPL-3.0-only

#pragma once

#include <cstdint>

struct Vector2;

extern bool vsync;
extern bool showFPS;
extern float panSensitivity;
extern float wheelSensitivity;
extern Vector2 mapSize;
extern double bestTime;
extern float musicVolume;
extern float sfxVolume;

void Save();
void Load();
