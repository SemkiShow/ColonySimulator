// SPDX-FileCopyrightText: 2025 Jaraslau Zaitsau
// SPDX-FileContributor: SemkiShow
//
// SPDX-License-Identifier: GPL-3.0-only

#include "Ship.hpp"
#include "Drawing.hpp"
#include "Island.hpp"
#include "Pathfinding.hpp"
#include "Perlin.hpp"
#include "Utils.hpp"
#include <raymath.h>
#include <vector>

#define SHIP_SPEED 25

std::vector<Ship> ships;

Ship::Ship(int sourceIndex, int targetIndex, int peopleCount)
    : sourceIndex(sourceIndex), targetIndex(targetIndex), people(peopleCount)
{
    Vector2 startPos = islands[sourceIndex].GetRandomPoint();
    Vector2 endPos = islands[targetIndex].GetRandomPoint();
    Vector2 dir = Vector2Normalize(endPos - startPos);

    while (path.size() <= 1)
    {
        do
        {
            startPos += dir;
        } while (GetPerlin(startPos) >= LAND_START);

        path = GetPath(startPos, targetIndex);
    }
    pos = path[0];

    nextPointDir = Vector2Normalize(path[0] - pos);
}

void Ship::Update()
{
    if (reached) return;
    Vector2 nextPos = pos + nextPointDir * SHIP_SPEED * GetFrameTime();
    if (Vector2Distance(pos, path[nextPointIdx]) > Vector2Distance(nextPos, path[nextPointIdx]))
    {
        pos = nextPos;
    }
    else
    {
        nextPointIdx++;
        if (nextPointIdx >= path.size())
        {
            reached = true;
            return;
        }

        nextPointDir = Vector2Normalize(path[nextPointIdx] - pos);
        if (nextPointDir.x > 0.1) flip = 1;
        if (nextPointDir.x < -0.1) flip = -1;
    }
}

void Ship::Draw()
{
    float scale = 0.01f / perlinScale;
    Vector2 position = GlslToRaylib(pos);
    DrawTexturePro(shipTexture, {0, 0, flip * shipTexture.width * 1.0f, shipTexture.height * 1.0f},
                   {position.x, position.y, shipTexture.width * scale, shipTexture.height * scale},
                   {shipTexture.width * scale / 2.0f, shipTexture.height * scale}, 0, WHITE);
}

Json Ship::ToJson()
{
    Json json;
    json["sourceIndex"] = sourceIndex;
    json["targetIndex"] = targetIndex;
    json["pos"] = Vector2ToJson(pos);
    json["flip"] = flip;

    json["path"].format = JsonFormat::Inline;
    for (auto& p: path)
    {
        json["path"].push_back(Vector2ToJson(p));
    }

    json["nextPointDir"] = Vector2ToJson(nextPointDir);
    json["nextPointIdx"] = (int)nextPointIdx;
    json["people"] = people;
    json["reached"] = reached;
    return json;
}

Ship Ship::LoadJson(Json& json)
{
    Ship ship;
    ship.sourceIndex = json["sourceIndex"].GetInt();
    ship.targetIndex = json["targetIndex"].GetInt();
    ship.pos = JsonToVector2(json["pos"]);
    ship.flip = json["flip"].GetInt();

    ship.path.clear();
    for (size_t i = 0; i < json["path"].size(); i++)
    {
        ship.path.push_back(JsonToVector2(json["path"][i]));
    }

    ship.nextPointDir = JsonToVector2(json["nextPointDir"]);
    ship.nextPointIdx = json["nextPointIdx"].GetInt();
    ship.people = json["people"].GetInt();
    ship.reached = json["reached"].GetBool();
    return ship;
}
