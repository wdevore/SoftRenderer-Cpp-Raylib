#pragma once

#include <string>
#include <ostream>

struct CColor
{
    int r;
    int g;
    int b;
    int a;

    friend std::ostream &operator<<(std::ostream &os, const CColor &c)
    {
        return os << std::to_string(c.r) + ", " + std::to_string(c.g) + ", " + std::to_string(c.b) + ", " + std::to_string(c.a);
    }
};
