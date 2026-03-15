#pragma once

#include "Object3D.h"
#include "WuColor.h"
#include "EdgeInterpolation.h"

class ShadeBase : public Object3D
{
private:
    /* data */
public:
    PaintColoring::WuColor wuColor{};
    PaintColoring::CColor color{};

    ShadeBase(/* args */);
    ~ShadeBase();

    void Initialize(PaintColoring::CColor bg, PaintColoring::CColor fg, int intensityBits) override;

    virtual bool IsMiddleLeft() = 0;
    virtual EdgeInterpolation &GetTopToMiddle() = 0;
    virtual EdgeInterpolation &GetTopToBottom() = 0;
    virtual EdgeInterpolation &GetMiddleToBottom() = 0;

    virtual void CalcVertexNormals() = 0;
};
