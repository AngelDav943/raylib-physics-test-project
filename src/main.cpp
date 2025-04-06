#include <iostream>
#include <raylib.h>
#include <list>
#include <vector>

#include "spaceObject.h"

ObjectManager space;

int main()
{
    const Color darkGreen = {20, 160, 133, 255};
#ifdef PLATFORM_DESKTOP
    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_VSYNC_HINT);
    InitWindow(800, 600, "Physics!");
    ChangeDirectory("assets");
    float cubeSize = 100.0f;
    float throwingForce = 5.0f;
#else
    InitWindow(0, 0, "Physics!");
    float cubeSize = 150.0f;
    float throwingForce = 10.0f;
#endif

    Texture2D cubeTexture = LoadTexture("ball.png");
    Texture2D bgTexture = LoadTexture("tiles.png");

    int spawnedAmount = 0;

    bool throwing = false;
    Vector2 startPosition = GetMousePosition();

    auto staticCube1 = new Cube();
    staticCube1->position = {75, 75};
    staticCube1->size = {cubeSize, cubeSize};
    space.AddElement("staticCube", staticCube1);
    
    auto staticCube2 = new Cube();
    staticCube2->position = {0, static_cast<float>(GetScreenHeight())};
    staticCube2->origin = {0.0f, 0.5f};
    staticCube2->size = {static_cast<float>(GetScreenWidth()), cubeSize};
    space.AddElement("staticCube", staticCube2);

    while (!WindowShouldClose())
    {
#ifdef PLATFORM_DESKTOP
        bool checkGestures = IsCursorOnScreen();
#else
        bool checkGestures = true;
#endif

        space.Update();

        BeginDrawing();
        ClearBackground(darkGreen);
        float bgWidthRepeats = (GetScreenWidth() / bgTexture.width) / 2;
        float bgHeightRepeats = (GetScreenHeight() / bgTexture.height) / 2;
        DrawTexturePro(bgTexture, {0, 0, (float)bgTexture.width * bgWidthRepeats, (float)bgTexture.height * bgHeightRepeats}, {0, 0, (float)GetRenderWidth(), (float)GetRenderHeight()}, {0, 0}, 0, WHITE);

        if (checkGestures == true)
        {
            if (IsGestureDetected(GESTURE_TAP))
            {
                startPosition = GetTouchPosition(0);
                throwing = true;
            }

            if (throwing)
            {
                Vector2 currentPosition = GetTouchPosition(0);
                Vector2 currentVelocity = Vector2Subtract(startPosition, currentPosition);
                Vector2 targetVelocity = Vector2ClampValue(currentVelocity, -200, 200);

                DrawLine(startPosition.x, startPosition.y, currentPosition.x, currentPosition.y, GREEN);

                if (IsGestureDetected(GESTURE_NONE))
                {
                    throwing = false;

                    auto spawnedCube = new PhysicsCube();
                    spawnedCube->position = startPosition;
                    spawnedCube->texture = cubeTexture;
                    spawnedCube->size = {cubeSize, cubeSize};
                    spawnedCube->velocity = Vector2Scale(targetVelocity, throwingForce);
                    // spawnedCube->hasGravity = false;
                    space.AddElement("spawned" + std::to_string(spawnedAmount), spawnedCube);
                    spawnedAmount++;
                }
            }
        }

        space.Draw();

#ifdef PLATFORM_DESKTOP
        DrawText("Running on Desktop", 10, 10, 20, BLUE);
#else
        DrawText("Running on Android", 10, 10, 20, RED);
#endif

        // if (GetTouchPointCount() > 0) {
        Vector2 pos = GetTouchPosition(0);
        list<string> texts = {
            ("GESTURE: " + to_string(GetGestureDetected())),
            ("POSX: " + to_string(pos.x)),
            ("POSY: " + to_string(pos.y))};

        Color textColor = BLUE;
        if (throwing)
            textColor = RED;

        int i = 0;
        for (auto objects : space.GetChildren())
        {
            i++;
            DrawText(objects->name.c_str(), 20, 20 + (texts.size() * 25) + (i * 25), 20, textColor);
        }

        i = 0;
        for (string text : texts)
        {
            i++;
            DrawText(text.c_str(), 20, 20 + (i * 25), 20, ORANGE);
        }

        EndDrawing();
    }

    CloseWindow();
}