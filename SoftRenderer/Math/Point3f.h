#pragma once

#include <string>

#include "Vector3f.h"

class Point3f
{
public:
    float x;
    float y;
    float z;

    Point3f();
    Point3f(float x, float y, float z);
    Point3f(const Point3f &p1);
    ~Point3f() = default;

    void set(float x, float y, float z);
    void set(const Point3f &p1);

    float distance(const Point3f &p1) const;
    float distanceSquared(const Point3f &p1) const;

    void add(const Vector3f &v1);
    void sub(const Vector3f &v1);
    void sub(const Point3f &p1, const Point3f &p2);

    bool equals(const Point3f &p1) const;
    bool epsilonEquals(const Point3f &p1, float epsilon) const;

    std::string toString() const;
};