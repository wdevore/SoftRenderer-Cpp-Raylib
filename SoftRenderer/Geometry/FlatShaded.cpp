#include "FlatShaded.h"

FlatShaded::FlatShaded(/* args */)
{
}

FlatShaded::FlatShaded(PaintColoring::WuColor wuColor, PaintColoring::CColor color)
{
    this->wuColor = wuColor;
    this->color = color;
}

FlatShaded::~FlatShaded()
{
}

void FlatShaded::CalcVertexNormals()
{
}

GradientInterpolation &FlatShaded::GetGradients(Vertex3f p1, Vertex3f p2, Vertex3f p3,
                                                int cI1, int cI2, int cI3)
{
    int ZTopIndex = 0;
    int ZMidIndex = 0;
    // sorted points
    Vertex3f spT{};
    Vertex3f spM{};
    Vertex3f spB{};

    if (p1.y < p2.y)
    {
        if (p3.y < p1.y)
        {
            // p1 < p2 and p3 < p1  	(3 < 1 < 2) Middle is Left
            spT = p3;
            spM = p1;
            spB = p2;
            ZTopIndex = 3;
            ZMidIndex = 1;
            middleIsLeft = true;
        }
        else
        {
            // p1 < p2 and p1 < p3
            if (p2.y < p3.y)
            {
                // p2 < p3  (1 < 2 < 3) Middle is Left
                spT = p1;
                spM = p2;
                spB = p3;
                ZTopIndex = 1;
                ZMidIndex = 2;
                middleIsLeft = true;
            }
            else
            {
                // p3 < p2  (1 < 3 < 2) Middle is Right
                spT = p1;
                spM = p3;
                spB = p2;
                ZTopIndex = 1;
                middleIsLeft = false;
            }
        }
    }
    else
    {
        if (p3.y < p2.y)
        {
            // p2 < p1 and p3 < p2   (3 < 2 < 1) Middle is Right
            spT = p3;
            spM = p2;
            spB = p1;
            ZTopIndex = 3;
            middleIsLeft = false;
        }
        else
        {
            // p2 < p1 and p2 < p3
            if (p1.y < p3.y)
            {
                // p1 < p3 		(2 < 1 < 3) Middle is Right
                spT = p2;
                spM = p1;
                spB = p3;
                ZTopIndex = 2;
                middleIsLeft = false;
            }
            else
            {
                // p3 < p1  	(2 < 3 < 1) Middle is Left
                spT = p2;
                spM = p3;
                spB = p1;
                ZTopIndex = 2;
                ZMidIndex = 3;
                middleIsLeft = true;
            }
        }
    }

    grd.Set(p1, p2, p3);

    // now setup the ShadeEdges
    TopToBottom.Set(grd, spT, spB, ZTopIndex);
    TopToMiddle.Set(grd, spT, spM, ZTopIndex);
    if (middleIsLeft)
        MiddleToBottom.Set(grd, spM, spB, ZMidIndex);
    else
        MiddleToBottom.Set(grd, spM, spB, ZTopIndex);

    return grd;
}
