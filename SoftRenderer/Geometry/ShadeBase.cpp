#include <iostream>

#include "ShadeBase.h"

ShadeBase::ShadeBase(/* args */)
{
}

ShadeBase::~ShadeBase()
{
}

void ShadeBase::Initialize(PaintColoring::CColor bg, PaintColoring::CColor fg, int intensityBits)
{
    color = fg;
    wuColor.initialize(bg, fg, intensityBits);
    name = "ShadeBase";
}
