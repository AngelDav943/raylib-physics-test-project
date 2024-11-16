#include <iostream>
#include <raylib.h>
#include <list>

#include "spaceObject.h"

ObjectManager space;

int main()
{
    const Color darkGreen = {20, 160, 133, 255};
#ifdef PLATFORM_DESKTOP
    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_VSYNC_HINT);
    InitWindow(800, 600, "Physics!");
#else
    InitWindow(0, 0, "Physics!");
#endif

    auto newCube = new PhysicsCube();
    newCube->mass = 0.1f;
    newCube->position = {0, 0};
    newCube->velocity = {100, 100};
    space.AddElement("cube", newCube);

    int spawnedAmount = 0;

    bool throwing = false;
    Vector2 startPosition = GetMousePosition();

    while (!WindowShouldClose())
    {
#ifdef PLATFORM_DESKTOP
        bool checkGestures = IsCursorOnScreen();
#else
        bool checkGestures = true;
#endif

        if (checkGestures == true)
        {
            // if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
            if (IsGestureDetected(GESTURE_TAP))
            {
                startPosition = GetTouchPosition(0);
                throwing = true;
            }

            if (throwing)
            {
                Vector2 currentPosition = GetTouchPosition(0);
                DrawLine(startPosition.x, startPosition.y, currentPosition.x, currentPosition.y, GREEN);

                if (IsGestureDetected(GESTURE_NONE))
                {
                    throwing = false;

                    auto spawnedCube = new PhysicsCube();
                    spawnedCube->position = startPosition;
                    spawnedCube->size = {100, 100};
                    spawnedCube->velocity = Vector2Scale(
                        Vector2Subtract(startPosition, currentPosition),
                        10);
                    // spawnedCube->hasGravity = false;
                    space.AddElement("spawned" + std::to_string(spawnedAmount), spawnedCube);
                    spawnedAmount++;
                }
            }
        }

        space.Update();

        BeginDrawing();
        ClearBackground(darkGreen);
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
            ("X: " + to_string(pos.x)),
            ("Y: " + to_string(pos.y)),
        };

        int i = 0;
        for (auto objects : space.GetChildren())
        {
            i++;
            DrawText(objects->name.c_str(), 20, 20 + (texts.size() * 25) + (i * 25), 20, BLUE);
        }

        i = 0;
        for (string text : texts)
        {
            i++;
            DrawText(text.c_str(), 20, 20 + (i * 25), 20, ORANGE);
        }
        // }

        EndDrawing();
    }

    CloseWindow();
}