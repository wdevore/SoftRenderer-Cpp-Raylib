#include <cmath>

#include "Plane.h"
#include "Constants.h"

Plane::Plane(/* args */)
{
}

Plane::~Plane()
{
}

void Plane::SetNormal(float x, float y, float z)
{
    normal.set(x, y, z);
    normal.normalize();
}

void Plane::SetPosition(float x, float y, float z)
{
    position.set(x, y, z);
}

void Plane::SetPosition(const Vector3f &v)
{
    position.set(v.x, v.y, v.z);
}

/// @brief Determine if point `p` is in front of plane or on the back side.
/// @param p point to be tested, for example p could be the camera's position.
/// @return
/// ```
/// 0 = coplanar, neither front or back
/// 1 = back side
/// 2 = front side
/// ```
int Plane::WhereIsPoint(const Vector3f &p)
{
    // In 3D mathematics, the "front" or "back" of a plane is determined by the
    // direction of its normal vector(n).
    // The standard way to check this is by taking the dot product between the
    // plane's normal and a vector pointing from the plane to your point (or the camera).
    // The Direction of View:
    // In rendering, if you are calculating the dot product between the
    // plane normal and the view vector (camera direction):
    // If n . V < 0, the plane is facing the camera (front-facing).
    // If n . V > 0, the plane is facing away from the camera (back-facing).

    // Create vector pointing from the plane to your point
    w.sub(p, position);

    float d = normal.dot(w);

    // 1) Front (Positive Half-space): If the dot product is greater than zero.
    // 2) Back (Negative Half-space): If the dot product is less than zero.
    // 3) On the Plane: If the dot product is exactly zero. (Coplanar)

    if (std::abs(d) < Maths::EPSILON)
    {
        // it is coplanar #3
        return 0;
    }

    if (d < 0.0f)
        return 1; // backside #2

    return 2; // frontside #3
}

int Plane::Intersect(const Vector3f &vP, const Vector3f &vQ)
{
    u.sub(vQ, vP);
    w.sub(vP, position);

    float d = normal.dot(u);
    float n = -normal.dot(w);

    if (std::abs(d) < Maths::EPSILON)
    {
        if (n == 0.0f)
            return 2;
        else
        {
            return 3; // default to backside.
        }
    }

    // They are not parallel. compute intersection
    float sI = n / d;
    if (sI < 0.0f || sI > 1.0f)
    {
        if (sI < 0.0f)
            return 3;
        else
            return 0;
    }

    // intersection = vP + sI*u
    intersection.set(vP);
    u.scale(sI);
    intersection.add(u);

    return 1;
}

int Plane::ClipToFront(const Vector3f &vP, const Vector3f &vQ, Vector3f &clP, Vector3f &clQ)
{
    // Clip line to a world plane
    int iP = WhereIsPoint(vP);
    int iQ = WhereIsPoint(vQ);

    if (iP == 1 && iQ == 1)
        return 0; // neither point was on the front side

    // One of the points is coplanar. The other on the backside.
    // So I consider this NOT visible.
    if (iP == 0 && iQ == 1)
        return 1;
    if (iP == 1 && iQ == 0)
        return 1;

    if ((iP == 0 && iQ == 2) || (iP == 2 && iQ == 0))
    {
        // One point is coplanar and the other is front. No cliping needed
        clP.set(vP);
        clQ.set(vQ); // End Point
    }
    else
    {
        int l = Intersect(vP, vQ);
        switch (l)
        {
        case 1:
            // Line intersects
            clQ.set(intersection); // New clipped End Point
            break;
        default:
            // Line is in front of plane
            clQ.set(vQ);
            break;
        }
        // We want only the point that was on the front of the plane
        if (iP == 2 || iP == 0)
        {
            clP.set(vP);
        }
        else if (iQ == 2 || iQ == 0)
        {
            // clP will be the front point.
            clP.set(vQ);
        }
    }

    // Indicate the segment was clipped.
    return 2;
}
