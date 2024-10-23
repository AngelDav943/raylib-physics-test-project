#pragma once
#include <raylib.h>

class Ball
{
public:
    Ball()
        : x(100), y(100), speedX(5), speedY(5), radius(15) {}

    void Update()
    {
        x += speedX;
        y += speedY;

        const int screenWidth = GetScreenWidth();
        const int screenHeight = GetScreenHeight();

        if ((x + radius >= screenWidth) || (x - radius <= 0))
        {
            speedX *= -1;
        }

        if ((y + radius >= screenHeight) || (y - radius <= 0))
        {
            speedY *= -1;
        }
    }

    void Draw() const
    {
        DrawCircle(x, y, radius, WHITE);
    }

private:
    int x;
    int y;
    int speedX;
    int speedY;
    int radius;
};