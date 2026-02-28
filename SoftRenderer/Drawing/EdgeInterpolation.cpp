#include <cmath>

#include "EdgeInterpolation.h"

EdgeInterpolation::EdgeInterpolation(/* args */)
{
}

EdgeInterpolation::~EdgeInterpolation()
{
}

void EdgeInterpolation::Set(GradientInterpolation g, Vertex3f top, Vertex3f bot, int ZTopIndex)
{
    Y = (int)std::ceil(top.y);
    int YEnd = (int)std::ceil(bot.y);
    Height = YEnd - Y;

    YPrestep = (float)Y - top.y;

    RealHeight = bot.y - top.y;
    RealWidth = bot.x - top.x;

    X = ((RealWidth * YPrestep) / RealHeight) + top.x;
    XStep = RealWidth / RealHeight;
    XPrestep = X - top.x;

    float aOneOverZ = 0.0f;
    switch (ZTopIndex)
    {
    case 1:
        aOneOverZ = g.aOneOverZ1;
        break;
    case 2:
        aOneOverZ = g.aOneOverZ2;
        break;
    case 3:
        aOneOverZ = g.aOneOverZ3;
        break;
    }

    OneOverZ = aOneOverZ + (YPrestep * g.dOneOverZdY) + (XPrestep * g.dOneOverZdX);

    OneOverZStep = (XStep * g.dOneOverZdX) + g.dOneOverZdY;
}

/// @brief This steps down the edge of the triangle, vertically and/or horizontally
/// @return
int EdgeInterpolation::Step()
{
    X += XStep;
    Y++;
    Height--; // This is a counter
    OneOverZ += OneOverZStep;
    return Height;
}
