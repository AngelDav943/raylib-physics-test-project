#include <iostream>
#include <raylib.h>

#include "spaceObject.h"

ObjectManager space;

int main()
{
    const Color darkGreen = {20, 160, 133, 255};
    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_VSYNC_HINT);
    InitWindow(800, 600, "Physics!");

    auto newCube = new PhysicsCube();
    space.AddElement("cube", newCube);

    while (!WindowShouldClose())
    {
        space.Update();

        BeginDrawing();
        ClearBackground(darkGreen);

        space.Draw();

        EndDrawing();
    }

    CloseWindow();
}