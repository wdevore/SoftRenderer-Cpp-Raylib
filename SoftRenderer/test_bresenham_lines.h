#pragma once

#include "raylib.h"

#include "Canvas.h"

class TestBresenhamLines
{
private:
    /* data */
public:
    TestBresenhamLines(/* args */);
    ~TestBresenhamLines();

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
