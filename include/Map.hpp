// SPDX-FileCopyrightText: 2026 SemkiShow
//
// SPDX-License-Identifier: GPL-3.0-only

#pragma once

#include "Drawing.hpp"
#include <atomic>
#include <vector>

std::vector<Vector2> GetBorderPoints(const std::vector<std::pair<int, int>>& islandPoints,
                                     float stepSize, float approxStepSize = 3);
void BuildIslands(std::atomic<bool>& finished, float stepSize = 0.1f);
void ResetMapVariables();
void BuildMap();
