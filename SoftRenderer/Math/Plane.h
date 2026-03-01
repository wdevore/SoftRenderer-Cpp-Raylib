#pragma once

#include "Vector3f.h"
#include "Point3f.h"

class Plane
{
private:
    /* data */
public:
    Vector3f normal{};
    Vector3f transformedNormal{};
    Point3f point{};
    Vector3f transformedPoint{};
    Vector3f intersection{};
    Vector3f u{};
    Vector3f w{};

    Plane(/* args */);
    ~Plane();

    void SetNormal(float x, float y, float z);
    void SetPoint(float x, float y, float z);
    void SetPoint(const Vector3f &v);
    void SetPoint(const Point3f &p);
    int WhereIsPoint(const Point3f &p);
    int Intersect(const Point3f &vP, const Point3f &vQ);
    int ClipToFront(Point3f vP, Point3f vQ, Point3f clP, Point3f clQ);
};
