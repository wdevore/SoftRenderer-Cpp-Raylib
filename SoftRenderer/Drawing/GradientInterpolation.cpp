#include "GradientInterpolation.h"

GradientInterpolation::GradientInterpolation(/* args */)
{
}

GradientInterpolation::~GradientInterpolation()
{
}

void GradientInterpolation::set(Vertex3f p1, Vertex3f p2, Vertex3f p3)
{
    X1 = p2.x - p3.x;
    X0 = p1.x - p3.x;
    Y1 = p2.y - p3.y;
    Y0 = p1.y - p3.y;
    float X1Y0 = X1 * Y0;
    float X0Y1 = X0 * Y1;

    //  				X1 		Y0  		X0  		Y1
    // 1/dx = 1 / [(x2 - x3)*(y1 - y3) - (x1 - x3)*(y2 - y3)]
    OneOverdX = 1.0f / (X1Y0 - X0Y1);
    OneOverdY = -OneOverdX;

    // Order dosen't matter here.
    //////////////////////////////////////////////////////////////////////
    // Z-Depth
    //////////////////////////////////////////////////////////////////////
    if (p1.z != 0.0f)
        aOneOverZ1 = 1.0f / p1.z;
    else
        aOneOverZ1 = 0.0f;
    if (p2.z != 0.0f)
        aOneOverZ2 = 1.0f / p2.z;
    else
        aOneOverZ2 = 0.0f;
    if (p3.z != 0.0f)
        aOneOverZ3 = 1.0f / p3.z;
    else
        aOneOverZ3 = 0.0f;

    /** dz/dx = 1/dx * (1/z2 - 1/z3)*(y1-y3) - (1/z1 - 1/z3)*(y2-y3)

        or     (1/z2 - 1/z3)*(y1-y3) - (1/z1 - 1/z3)*(y2-y3)
              ----------------------------------------------
                 (x2 - x3)*(y1 - y3) - (x1 - x3)*(y2 - y3)
    */
    float d1 = aOneOverZ2 - aOneOverZ3;
    float d2 = aOneOverZ1 - aOneOverZ3;
    aaYbbY = (d1 * Y0) - (d2 * Y1);
    dOneOverZdX = OneOverdX * aaYbbY;

    /**
                (1/z2 - 1/z3)*(x1-x3) - (1/z1 - 1/z3)*(x2-x3)
            -----------------------------------------------------
                     -[(x2-x3)*(y1-y3) - (x1-x3)*(y2-y3)]
    */
    aaXbbX = (d1 * X0) - (d2 * X1);
    dOneOverZdY = OneOverdY * aaXbbX;
}
