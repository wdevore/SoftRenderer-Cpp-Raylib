#include "CColor.h"

void PaintColoring::CColor::Set(CColor color)
{
    r = color.r;
    g = color.g;
    b = color.b;
    a = color.a;
}

void PaintColoring::CColor::Set(int r, int g, int b, int a)
{
    this->r = r;
    this->g = g;
    this->b = b;
    this->a = a;
}
