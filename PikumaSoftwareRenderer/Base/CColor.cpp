#include "CColor.h"

void CColor::Set(CColor color)
{
    r = color.r;
    g = color.g;
    b = color.b;
    a = color.a;
}

void CColor::Set(int r, int g, int b, int a)
{
    this->r = r;
    this->g = g;
    this->b = b;
    this->a = a;
}

CColor::CColor(uint color)
{
    Set(color);
}

void CColor::Set(uint color)
{
    a = (color >> 24) & 0xFF;
    r = (color >> 16) & 0xFF;
    g = (color >> 8) & 0xFF;
    b = color & 0xFF;
}

void CColor::SetFromUint32(uint32_t color)
{
    Set((uint)color);
}
