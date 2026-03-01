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
    void set(const Point3f &p);
    void set(const Vector3f &p);

    float distance(const Point3f &p) const;
    float distanceSquared(const Point3f &p) const;

    void add(const Vector3f &v);
    void sub(const Vector3f &v);
    void sub(const Point3f &p1, const Point3f &p2);
    void scale(float s);

    bool equals(const Point3f &p1) const;
    bool epsilonEquals(const Point3f &p1, float epsilon) const;

    std::string toString() const;
};