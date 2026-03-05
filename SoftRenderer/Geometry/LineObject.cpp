#include "LineObject.h"

LineObject::LineObject(PaintColoring::CColor bg, PaintColoring::CColor fg)
{
    this->color = fg;
    wuColor.initialize(bg, fg, 8);
    name = "LineObject";
}

LineObject::~LineObject()
{
}

void LineObject::SetAnimate(bool animate)
{
    this->animate = animate;
}
