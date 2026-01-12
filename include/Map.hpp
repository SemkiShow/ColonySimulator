// SPDX-FileCopyrightText: 2026 SemkiShow
//
// SPDX-License-Identifier: GPL-3.0-only

#pragma once

#include <atomic>

void BuildIslands(std::atomic<bool>& finished, float stepSize = 0.1f);
void BuildMap();
