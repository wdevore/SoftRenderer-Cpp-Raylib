#pragma once

#include "Object3D.h"
#include "WuColor.h"
#include "CColor.h"

class LineObject : public Object3D
{
private:
    /* data */
public:
    PaintColoring::WuColor wuColor{};
    PaintColoring::CColor color{};

    LineObject(PaintColoring::CColor bg, PaintColoring::CColor fg);
    ~LineObject();

    void SetAnimate(bool animate) override;
};
