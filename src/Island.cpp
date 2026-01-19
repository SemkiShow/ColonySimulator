// SPDX-FileCopyrightText: 2025 SemkiShow
// SPDX-FileContributor: Jaraslau Zaitsau
//
// SPDX-License-Identifier: GPL-3.0-only

#include "Island.hpp"
#include "Drawing.hpp"
#include "Human.hpp"
#include "Pathfinding.hpp"
#include "Perlin.hpp"
#include "Settings.hpp"
#include "Ship.hpp"
#include "Utils.hpp"
#include <RCore/Translations.hpp>
#include <cmath>
#include <raymath.h>

#define K_WOOD_GET 3
#define K_IRON_GET 1
#define K_EFFICIENCY 5

inline Color rgb(unsigned char r, unsigned char g, unsigned char b) { return {r, g, b, 255}; }

std::vector<Biome> biomes = {{-1, rgb(0, 0, 255)},     {-0.5, rgb(0, 136, 255)},
                             {0, rgb(97, 218, 255)},   {0.1, rgb(251, 254, 145)},
                             {0.2, rgb(33, 171, 42)},  {0.5, rgb(184, 184, 205)},
                             {0.6, rgb(255, 255, 255)}};
std::vector<Island> islands;

int woodTotal = 0, ironTotal = 0, peopleTotal = 0;

Vector2 Island::GetRandomPoint()
{
    Vector2 pos;
    do
    {
        pos.x = GetRandomFloat(p1.x, p2.x);
        pos.y = GetRandomFloat(p1.y, p2.y);
    } while (GetPerlin(pos) < LAND_START);
    return pos;
}

void Island::Colonize()
{
    if (colonized || colonizationInProgress || woodTotal < woodColonize || ironTotal < ironColonize)
        return;
    colonizationInProgress = true;
    woodTotal -= woodColonize;
    ironTotal -= ironColonize;
    SendPeople(1);
}

void Island::SendPeople(int count)
{
    if (futurePeopleCount + count > peopleMax) return;
    int maxPeopleIslandId = (&islands[0] == this ? 1 : 0);
    for (size_t i = 0; i < islands.size(); i++)
    {
        if (&islands[i] == this) continue;
        if (islands[i].peopleCount > islands[maxPeopleIslandId].peopleCount) maxPeopleIslandId = i;
    }
    if (islands[maxPeopleIslandId].peopleCount < count) return;
    islands[maxPeopleIslandId].peopleCount -= count;
    futurePeopleCount += count;
    ships.emplace_back(islands[maxPeopleIslandId].index, this->index, count);

    int counter = 0;
    for (auto it = people.begin(); it != people.end();)
    {
        if (counter >= count) break;
        if (it->islandIdx != maxPeopleIslandId)
        {
            ++it;
            continue;
        }
        it = people.erase(it);
        counter++;
    }
}

void Island::AddPeople(int count)
{
    if (!colonized) colonized = true;
    peopleCount += count;
    for (int i = 0; i < count; i++)
    {
        people.emplace_back(GetRandomPoint(), index);
    }
}

void Island::GrowthTick()
{
    if (!colonized) return;
    woodCount += woodGrowth;
    woodCount = fmin(woodCount, woodMax);
    if (peopleCount >= 2)
    {
        addPeopleFraction += peopleGrowth * sqrt(peopleCount) * efficiency / 100;

        int delta = addPeopleFraction;
        delta = fmin(delta, peopleMax - peopleCount);
        addPeopleFraction -= delta;
        peopleCount += delta;
        peopleTotal += delta;
        for (int i = 0; i < delta; i++)
        {
            people.emplace_back(GetRandomPoint(), index);
        }
    }
    {
        int delta = fmin(woodCount, K_WOOD_GET * peopleCount * taxes / 100 * efficiency / 100);
        woodCount -= delta;
        woodTotal += delta;
    }
    {
        int delta = fmin(ironCount, K_IRON_GET * peopleCount * taxes / 100 * efficiency / 100);
        ironCount -= delta;
        ironTotal += delta;
    }
    if (taxes < DEFAULT_TAXES)
    {
        efficiency += rand() % (DEFAULT_TAXES - taxes) / K_EFFICIENCY;
        efficiency = fmin(100, efficiency);
    }
    if (taxes > DEFAULT_TAXES)
    {
        efficiency -= rand() % (taxes - DEFAULT_TAXES) / K_EFFICIENCY;
        efficiency = fmax(0, efficiency);
    }
}

void Island::DrawStats()
{
    // Do not draw anything if the scale is too small
    float scale = 0.01f / perlinScale;
    if (scale < 0.05f) return;

    // Constants
    const float margin = 100 * scale, lockScale = 0.5f * scale, woodScale = 0.15f * scale,
                ironScale = 0.15f * scale, humanScale = 0.075f * scale, textScale = 175 * scale,
                buttonScale = 300 * scale;

    // Calculate the center point of the island
    Vector2 center = GlslToRaylib((p2 + p1) / 2);
    center.x -= lockTexture.width * scale / 2;
    center.y -= lockTexture.height * scale / 2;

    // Calculate the offset
    Vector2 offset = {0, -lockTexture.height * lockScale / 2};
    if (colonized) offset *= -1;

    // Draw lock
    if (!colonized)
    {
        DrawTextureEx(lockTexture, center + offset, 0, lockScale, WHITE);
        offset.y += lockTexture.height * lockScale + margin;
    }

    auto GetTextOffset = [&](const Texture& texture, float textureScale) -> Vector2
    {
        Vector2 newOffset = center + offset;
        newOffset.x +=
            fmax(woodTexture.width * woodScale, ironTexture.width * ironScale) / 2 + margin;
        newOffset.y += (texture.height * textureScale - textScale) * scale / 2;
        return newOffset;
    };

    // Draw a dark background for better text visibility
    Rectangle rec = {center.x + offset.x - lockTexture.width * lockScale / 2,
                     center.y + offset.y - margin / 2, lockTexture.width * lockScale * 2,
                     woodTexture.height * woodScale + ironTexture.height * ironScale + margin * 2 +
                         (colonized ? humanTexture.height * humanScale + margin : 0)};
    DrawRectangleRounded(rec, 0.25f, 16, {0, 0, 0, 127});
    if (!colonized) DrawRectangleRoundedLinesEx(rec, 0.25f, 16, 3, RED);

    // Draw wood
    DrawTextureEx(woodTexture, center + offset - Vector2{woodTexture.width * woodScale / 2, 0}, 0,
                  woodScale, WHITE);
    {
        Vector2 textOffset = GetTextOffset(woodTexture, woodScale);
        DrawTextCustom(std::to_string(colonized ? woodCount : woodColonize).c_str(), textOffset,
                       textScale, WHITE);
    }
    offset.y += woodTexture.height * woodScale + margin;

    // Draw iron
    DrawTextureEx(ironTexture, center + offset - Vector2{ironTexture.width * ironScale / 2, 0}, 0,
                  ironScale, WHITE);
    {
        Vector2 textOffset = GetTextOffset(ironTexture, ironScale);
        DrawTextCustom(std::to_string(colonized ? ironCount : ironColonize).c_str(), textOffset,
                       textScale, WHITE);
    }
    offset.y += ironTexture.height * ironScale + margin;

    // Draw people
    if (colonized)
    {
        DrawTextureEx(humanTexture,
                      center + offset - Vector2{humanTexture.width * humanScale / 2, 0}, 0,
                      humanScale, WHITE);
        {
            Vector2 textOffset = GetTextOffset(humanTexture, humanScale);
            DrawTextCustom(std::to_string(peopleCount).c_str(), textOffset, textScale, WHITE);
        }
        offset.y += humanTexture.height * humanScale + margin;
    }

    // Draw taxes button
    if (colonized)
    {
        auto buttonRec = rec;
        buttonRec.width = buttonRec.height = buttonScale;
        buttonRec.x += rec.width;
        // if (GuiButton(buttonRec, "#142#")) islandEditIdx = index;
    }
}

Json Island::ToJSON()
{
    Json json;

    json["p1"].format = JsonFormat::Inline;
    json["p1"].push_back(p1.x);
    json["p1"].push_back(p1.y);

    json["p2"].format = JsonFormat::Inline;
    json["p2"].push_back(p2.x);
    json["p2"].push_back(p2.y);

    json["area"] = area;
    json["woodColonize"] = woodColonize;
    json["ironColonize"] = ironColonize;
    json["woodCount"] = woodCount;
    json["woodGrowth"] = woodGrowth;
    json["woodMax"] = woodMax;
    json["ironCount"] = ironCount;
    json["peopleCount"] = peopleCount;
    json["peopleMax"] = peopleMax;
    json["peopleGrowth"] = peopleGrowth;
    json["addPeopleFraction"] = addPeopleFraction;
    json["colonized"] = colonized;
    json["taxes"] = taxes;
    json["efficiency"] = efficiency;

    return json;
}

Island Island::LoadJSON(Json& json)
{
    Island island;
    island.p1 = {static_cast<float>(json["p1"][0].GetDouble()),
                 static_cast<float>(json["p1"][1].GetDouble())};
    island.p2 = {static_cast<float>(json["p2"][0].GetDouble()),
                 static_cast<float>(json["p2"][1].GetDouble())};
    island.area = json["area"].GetDouble();
    island.woodColonize = json["woodColonize"].GetInt();
    island.ironColonize = json["ironColonize"].GetInt();
    island.woodCount = json["woodCount"].GetInt();
    island.woodGrowth = json["woodGrowth"].GetInt();
    island.woodMax = json["woodMax"].GetInt();
    island.ironCount = json["ironCount"].GetInt();
    island.peopleCount = json["peopleCount"].GetInt();
    island.peopleMax = json["peopleMax"].GetInt();
    island.peopleGrowth = json["peopleGrowth"].GetDouble();
    island.addPeopleFraction = json["addPeopleFraction"].GetDouble();
    island.colonized = json["colonized"].GetBool();
    island.taxes = json["taxes"].GetInt();
    island.efficiency = json["efficiency"].GetInt();
    return island;
}
