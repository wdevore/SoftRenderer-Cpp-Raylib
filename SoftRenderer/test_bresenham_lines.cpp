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
    canvas.DrawBresenhamLine(pxs, pys, pxe, pye, ORANGE);

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