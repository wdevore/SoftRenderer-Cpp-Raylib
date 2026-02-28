#pragma once

#include <iomanip>

#include "Vertex3f.h"

class GradientInterpolation
{
private:
    /* data */
public:
    float OneOverdX{};
    float OneOverdY{};
    float X1{};
    float X0{};
    float Y1{};
    float Y0{};
    float aaXbbX{};
    float aaYbbY{};
    float aOneOverZ1{};
    float aOneOverZ2{};
    float aOneOverZ3{};
    float dOneOverZdX, dOneOverZdY{};

    int intGradientType{};

    GradientInterpolation(/* args */);
    ~GradientInterpolation();

    void set(Vertex3f p1, Vertex3f p2, Vertex3f p3);

    friend std::ostream &operator<<(std::ostream &os, const GradientInterpolation &g)
    {
        return os << std::fixed << std::setprecision(2)
                  << "dOneOverZdX= " << g.dOneOverZdX
                  << ", dOneOverZdY= " << g.dOneOverZdY;
    }
};
