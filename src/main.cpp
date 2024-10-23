#include <iostream>
#include <raylib.h>
#include "ball.h"


int main() 
{
    const Color darkGreen = {20, 160, 133, 255};
    
    constexpr int screenWidth = 800;
    constexpr int screenHeight = 600;
    
    Ball ball;
    
    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_VSYNC_HINT);
    InitWindow(screenWidth, screenHeight, "My first RAYLIB program!");
    SetTargetFPS(60);

    Rectangle rec = {
        .x = 100,
        .y = 10,
        .width = 100,
        .height = 200
    };

    std::cout << "Rec" << rec.x << std::endl;
    
    while (!WindowShouldClose())
    {
        ball.Update();
        
        BeginDrawing();
            ClearBackground(darkGreen);
            DrawRectanglePro(rec, {0,0}, 45, ORANGE);
            ball.Draw();
        EndDrawing();
    }
    
    CloseWindow();
}