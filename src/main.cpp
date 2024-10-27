#include <iostream>
#include <raylib.h>

#include "spaceObject.h"

ObjectManager space;

int main()
{
    const Color darkGreen = {20, 160, 133, 255};
    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_VSYNC_HINT);
    InitWindow(800, 600, "Physics!");

    // auto newCube = new PhysicsCube();
    // newCube->mass = 0.1f;
    // newCube->position = {0, 0};
    // space.AddElement("cube", newCube);

    int spawnedAmount = 0;

    bool throwing = false;
    Vector2 startPosition = GetMousePosition();

    while (!WindowShouldClose())
    {

        if (IsCursorOnScreen())
        {
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
            {
                startPosition = GetMousePosition();
                throwing = true;
            }

            if (throwing)
            {
                Vector2 currentPosition = GetMousePosition();
                DrawLine(startPosition.x, startPosition.y, currentPosition.x, currentPosition.y, GREEN);

                if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON))
                {
                    throwing = false;

                    auto spawnedCube = new PhysicsCube();
                    spawnedCube->position = startPosition;
                    spawnedCube->size = {100, 100};
                    spawnedCube->velocity = Vector2Scale(
                        Vector2Subtract(startPosition, currentPosition),
                        10
                        );
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

        int i = 0;
        for (auto objects : space.GetChildren())
        {
            i++;
            DrawText(objects->name.c_str(), 20, 20 + (i * 25), 20, ORANGE);
        }

        EndDrawing();
    }

    CloseWindow();
}