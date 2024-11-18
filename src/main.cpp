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
#else
    InitWindow(0, 0, "Physics!");
#endif

    Texture2D cubeTexture = LoadTexture("ball.png");
    Texture2D bgTexture = LoadTexture("tiles.png");

    int spawnedAmount = 0;

    bool dragging = false;
    bool throwing = false;
    Vector2 startPosition = GetMousePosition();
    Vector2 lastPosition;

    PhysicsCube *selectedCube = nullptr;

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
            // if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))

            // if (selectedCube == nullptr && (IsGestureDetected(GESTURE_TAP) || IsGestureDetected(GESTURE_HOLD) || IsGestureDetected(GESTURE_DRAG)))
            // {
            //     vector<Object *> children = space.GetChildren();
            //     for (size_t i = 0; i < children.size(); i++)
            //     {
            //         if (PhysicsCube *element = dynamic_cast<PhysicsCube *>(children[i]))
            //         {
            //             Rectangle rec = {
            //                 element->position.x,
            //                 element->position.y,
            //                 element->size.x,
            //                 element->size.y};

            //             bool selecting = CheckCollisionPointRec(GetTouchPosition(0), rec);
            //             if (selecting == true)
            //             {
            //                 selectedCube = element;
            //             }
            //         }
            //     }
            //     if (selectedCube != nullptr)
            //     {
            //         selectedCube->hasGravity = false;
            //         lastPosition = GetTouchPosition(0);
            //         dragging = true;
            //     }
            // }

            if (IsGestureDetected(GESTURE_TAP) && selectedCube == nullptr)
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
                    spawnedCube->size = {100, 100};
                    spawnedCube->velocity = Vector2Scale(targetVelocity, 5);
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
            ("POSY: " + to_string(pos.y))
        };

        if (selectedCube != nullptr)
        {
            texts.push_front("Selected: " + selectedCube->name);
        }

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