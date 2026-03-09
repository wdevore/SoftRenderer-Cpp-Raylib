#pragma once

#include <ostream>

#include "Point3f.h"

class Vertex3f : public Point3f
{
private:
public:
    bool transformed{false};

    Vertex3f(/* args */);
    Vertex3f(float x, float y, float z);

    ~Vertex3f();

    friend std::ostream &operator<<(std::ostream &os, const Vertex3f &o)
    {
        return os << "<" << o.x << " : " << o.y << " : " << o.z << ">";
    }
};
