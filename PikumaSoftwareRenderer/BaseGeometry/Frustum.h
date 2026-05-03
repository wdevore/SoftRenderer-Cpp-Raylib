#pragma once

#include <vector>

#include "Plane.h"
#include "Polygon.h"

///////////////////////////////////////////////////////////////////////////////
// Frustum planes are defined by a point and a normal vector
///////////////////////////////////////////////////////////////////////////////
// Near plane   :  P=(0, 0, znear), N=(0, 0,  1)
// Far plane    :  P=(0, 0, zfar),  N=(0, 0, -1)
// Top plane    :  P=(0, 0, 0),     N=(0, -cos(fovy/2), sin(fovy/2))
// Bottom plane :  P=(0, 0, 0),     N=(0, cos(fovy/2), sin(fovy/2))
// Left plane   :  P=(0, 0, 0),     N=(cos(fovx/2), 0, sin(fovx/2))
// Right plane  :  P=(0, 0, 0),     N=(-cos(fovx/2), 0, sin(fovx/2))
///////////////////////////////////////////////////////////////////////////////
//
//           /|\
//         /  | |
//       /\   | |
//     /      | |
//  P*|-->  <-|*|   ----> +z-axis
//     \      | |
//       \/   | |
//         \  | |
//           \|/
//
///////////////////////////////////////////////////////////////////////////////

namespace Geometry
{
    enum FrustumPlane
    {
        LEFT,
        RIGHT,
        TOP,
        BOTTOM,
        NEAR,
        FAR
    };

    class Frustum
    {
    private:
        /* data */
    public:
        std::vector<Plane> planes{};

        std::vector<Maths::Vector3f> insideVertices{};
        std::vector<Maths::Texture2f> insideTexcoords{};

        Frustum(/* args */);
        ~Frustum();

        void initialize(float fovX, float fovY, float znear, float zfar);

        void clip(Polygon &polygon);
        void clipAgainstPlane(Polygon &polygon, FrustumPlane plane);
    };

} // namespace Geometry
