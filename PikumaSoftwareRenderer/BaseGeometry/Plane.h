#pragma once

#include "Vector3f.h"

namespace Geometry
{
    class Plane
    {
    private:
        /* data */
    public:
        Maths::Vector3f point{};
        Maths::Vector3f normal{};

        Plane(/* args */);
        ~Plane();
    };

} // namespace Geometry
