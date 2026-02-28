#pragma once

#include <string>
#include <ostream>
#include <iomanip>

#include "Vertex3f.h"
#include "GradientInterpolation.h"

class EdgeInterpolation
{
private:
    /* data */
public:
    float X, XStep{}; // fractional x and dX/dY
    int Y, Height{};  // current y and vertical count
    float OneOverZ;
    float OneOverZStep{}; // 1/z and step
    float YPrestep{};
    float XPrestep{};
    float RealHeight{};
    float RealWidth{};

    EdgeInterpolation(/* args */);
    ~EdgeInterpolation();

    void Set(GradientInterpolation g, Vertex3f top, Vertex3f bot, int ZTopIndex);
    int Step();

    friend std::ostream &operator<<(std::ostream &os, const EdgeInterpolation &e)
    {
        return os << std::fixed << std::setprecision(2)
                  << "X= " << e.X
                  << ", XStep= " << e.XStep
                  << ", 1/z= " << e.OneOverZ
                  << ", 1/zStep= " << e.OneOverZStep
                  << ", Y= " << e.Y
                  << ", Height= " << e.Height
                  << ", YPrestep= " << e.YPrestep
                  << ", XPrestep= " << e.XPrestep;
    }
};
