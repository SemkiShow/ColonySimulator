// SPDX-FileCopyrightText: 2025 SemkiShow
//
// SPDX-License-Identifier: GPL-3.0-only

#include "Human.hpp"
#include "Drawing.hpp"
#include "Island.hpp"
#include "Perlin.hpp"
#include <raymath.h>

#define MIN_ANGLE -15
#define MAX_ANGLE 15

std::vector<Human> people;

void Human::Update()
{
    double deltaTime = GetFrameTime();

    Vector2 delta = Vector2Rotate({speed, 0}, rotation) * deltaTime;
    bool found = false;
    for (size_t i = 0; i < 5; i++)
    {
        if (GetPerlin(pos + delta) >= LAND_START && InsideMap(pos + delta))
        {
            found = true;
            break;
        }
        rotation = GetRandomFloat(0, 360);
        delta = Vector2Rotate({speed, 0}, rotation) * deltaTime;
    }
    if (found) pos += delta;

    angle += angleMultiplier * rotationSpeed * deltaTime;
    if (angle < MIN_ANGLE) angleMultiplier = 1;
    if (angle > MAX_ANGLE) angleMultiplier = -1;
    angle = fmax(MIN_ANGLE, fmin(MAX_ANGLE, angle));
}

void Human::Draw()
{
    float scale = 0.0005f / perlinScale;
    Vector2 position = GlslToRaylib(pos);
    DrawTexturePro(
        humanTexture, {0, 0, humanTexture.width * 1.0f, humanTexture.height * 1.0f},
        {position.x, position.y, humanTexture.width * scale, humanTexture.height * scale},
        {humanTexture.width * scale / 2.0f, humanTexture.height * scale}, angle, WHITE);
}

Json Human::ToJSON()
{
    Json json;

    json["pos"] = Vector2ToJson(pos);
    json["angle"] = angle;
    json["rotation"] = rotation;
    json["islandIdx"] = islandIdx;
    json["speed"] = speed;
    json["rotationSpeed"] = rotationSpeed;

    return json;
}

Human Human::LoadJSON(Json& json)
{
    Human human;

    human.pos = JsonToVector2(json["pos"]);
    human.angle = json["angle"].GetDouble();
    human.rotation = json["rotation"].GetDouble();
    human.islandIdx = json["islandIdx"].GetInt();
    human.speed = json["speed"].GetDouble();
    human.rotationSpeed = json["rotationSpeed"].GetDouble();

    return human;
}
