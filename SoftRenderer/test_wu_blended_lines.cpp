#include <iostream>
#include "test_wu_blended_lines.h"

TestWublendedLines::TestWublendedLines(/* args */)
{
}

TestWublendedLines::~TestWublendedLines()
{
}

void TestWublendedLines::initialize(int width, int height)
{
    this->width = width;
    this->height = height;
    pxs = GetRandomValue(0, width - 1);
    pys = GetRandomValue(0, height - 1);
    pxe = GetRandomValue(0, width - 1);
    pye = GetRandomValue(0, height - 1);
    delay = 60;
    delayCnt = 0;
}

void TestWublendedLines::draw(Canvas &canvas)
{

    // canvas.DrawWuBlendedLine(10, 200, 210, 200, wuColor);
    // canvas.DrawWuBlendedLine(200, 10, 200, 210, wuColor);
    // canvas.DrawWuBlendedLine(10, 10, 200, 200, wuColor);

    delayCnt++;
    if (delayCnt >= delay)
    {
        delayCnt = 0;
        pxs = GetRandomValue(0, width - 1);
        pys = GetRandomValue(0, height - 1);
        pxe = GetRandomValue(0, width - 1);
        pye = GetRandomValue(0, height - 1);
        // std::cout << pxs << " " << pys << " : " << pxe << " " << pye << std::endl;
    }
}