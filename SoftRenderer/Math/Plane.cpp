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
    transformedNormal.set(normal);
}

void Plane::SetPoint(float x, float y, float z)
{
    point.set(x, y, z);
    transformedPoint.set(point);
}

void Plane::SetPoint(const Vector3f &v)
{
    point.set(v.x, v.y, v.z);
    transformedPoint.set(point);
}

void Plane::SetPoint(const Point3f &p)
{
    point.set(p);
    transformedPoint.set(point);
}

/// @brief Determine if point `p` is in front of plane or on the back side.
/// @param p point to be tested
/// @return
/// ```
/// 0 = coplanar, neither front or back
/// 1 = back side
/// 2 = front side
/// ```
int Plane::WhereIsPoint(const Point3f &p)
{
    w.sub(p, transformedPoint);
    float d = transformedNormal.dot(w);
    if (std::abs(d) < Maths::EPSILON)
    {
        // it is coplanar
        return 0;
    }
    if (d < 0.0f)
        return 1; // backside
    return 2;     // frontside
}

int Plane::Intersect(const Point3f &vP, const Point3f &vQ)
{
    u.sub(vQ, vP);
    w.sub(vP, transformedPoint);

    float d = transformedNormal.dot(u);
    float n = -transformedNormal.dot(w);

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

int Plane::ClipToFront(Point3f vP, Point3f vQ, Point3f clP, Point3f clQ)
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
