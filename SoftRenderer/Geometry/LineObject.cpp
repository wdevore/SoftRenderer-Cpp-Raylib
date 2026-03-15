#include "LineObject.h"

LineObject::LineObject(PaintColoring::CColor bg, PaintColoring::CColor fg)
{
    type = ObjectType::Line;
    this->color = fg;
    wuColor.initialize(bg, fg, 8);
    name = "LineObject";
}

LineObject::~LineObject()
{
}

void LineObject::Initialize(PaintColoring::CColor bg, PaintColoring::CColor fg, int intensityBits)
{
}

void LineObject::SetAnimate(bool animate)
{
    this->animate = animate;
}
