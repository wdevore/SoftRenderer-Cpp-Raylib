#include "test_bresenham_lines.h"

TestBresenhamLines::TestBresenhamLines(/* args */)
{
}

TestBresenhamLines::~TestBresenhamLines()
{
}

void TestBresenhamLines::initialize(int width, int height)
{
    this->width = width;
    this->height = height;
    pxs = GetRandomValue(0, width);
    pys = GetRandomValue(0, height);
    pxe = GetRandomValue(0, width);
    pye = GetRandomValue(0, height);
    delay = 30;
    delayCnt = 0;
}

void TestBresenhamLines::draw(Canvas &canvas)
{
    WuColor wuColor{};
    CColor bg{};
    bg.r = 80;
    bg.g = 80;
    bg.b = 80;
    bg.a = 255;

    CColor fg{};
    fg.r = 255;
    fg.g = 161;
    fg.b = 0;
    fg.a = 255;

    wuColor.initialize(bg, fg, 16);

    // canvas.DrawZBresenhamLine(pxs, pys, pxe, pye, 10.0f, 10.0f, wuColor);

    canvas.DrawZBresenhamLine(pxs, pys, pxe, pye, 10.0f, 10.0f, ORANGE);
    // If zQ is smaller GREEN line appears over ORANGE.
    canvas.DrawZBresenhamLine(pxs, pys - 5, pxe, pye + 5, 10.0f, 9.0f, GREEN);

    // canvas.DrawBresenhamLine(pxs, pys, pxe, pye, ORANGE);

    delayCnt++;
    if (delayCnt >= delay)
    {
        delayCnt = 0;
        pxs = GetRandomValue(0, width);
        pys = GetRandomValue(0, height);
        pxe = GetRandomValue(0, width);
        pye = GetRandomValue(0, height);
    }
}