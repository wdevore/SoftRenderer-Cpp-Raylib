#pragma once

#include "ShadeBase.h"
#include "GradientInterpolation.h"
#include "EdgeInterpolation.h"
#include "WuColor.h"

class FlatShaded : public ShadeBase
{
private:
    GradientInterpolation grd{};
    EdgeInterpolation MiddleToBottom{};
    EdgeInterpolation TopToBottom{};
    EdgeInterpolation TopToMiddle{};
    bool middleIsLeft{};

public:
    FlatShaded();
    FlatShaded(PaintColoring::WuColor wuColor, PaintColoring::CColor color);
    ~FlatShaded();

    bool IsMiddleLeft() override { return middleIsLeft; };
    EdgeInterpolation &GetTopToMiddle() override { return TopToMiddle; };
    EdgeInterpolation &GetTopToBottom() override { return TopToBottom; };
    EdgeInterpolation &GetMiddleToBottom() override { return MiddleToBottom; };
    void SetAnimate(bool animate) override { this->animate = animate; };

    void CalcVertexNormals() override;

    GradientInterpolation &GetGradients(Vertex3f p1, Vertex3f p2, Vertex3f p3,
                                        int cI1, int cI2, int cI3);
};
