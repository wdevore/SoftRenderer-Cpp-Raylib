#pragma once

#include "Vector3f.h"

class Plane
{
private:
    /* data */
public:
    Vector3f normal{};
    Vector3f position{};
    Vector3f intersection{};
    Vector3f u{};
    Vector3f w{};

    Plane(/* args */);
    ~Plane();

    void SetNormal(float x, float y, float z);
    void SetPosition(float x, float y, float z);
    void SetPosition(const Vector3f &v);

    int WhereIsPoint(const Vector3f &p);
    int Intersect(const Vector3f &vP, const Vector3f &vQ);
    int ClipToFront(const Vector3f &vP, const Vector3f &vQ, Vector3f &clP, Vector3f &clQ);
};
