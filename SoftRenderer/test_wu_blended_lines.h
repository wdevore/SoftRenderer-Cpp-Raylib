#pragma once

#include "raylib.h"

#include "Canvas.h"

class TestWublendedLines
{
private:
    /* data */
public:
    TestWublendedLines(/* args */);
    ~TestWublendedLines();

    int width{};
    int height{};

    int pxs{};
    int pys{};
    int pxe{};
    int pye{};
    int delay{};
    int delayCnt{};

    void initialize(int width, int height);
    void draw(Canvas &canvas);
};
