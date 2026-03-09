#pragma once
#include <string>
#include <ostream>

class Point3f;

class Vector3f
{
public:
    float x;
    float y;
    float z;

    Vector3f();
    Vector3f(float x, float y, float z);
    Vector3f(const Vector3f &v1);
    ~Vector3f() = default;

    void set(float x, float y, float z);
    void set(const Vector3f &v);
    void set(const Point3f &p);

    float length() const;
    float lengthSquared() const;
    void normalize();
    float dot(const Vector3f &v1) const;
    void cross(const Vector3f &v1, const Vector3f &v2);

    void add(const Vector3f &v1);
    void add(const Vector3f &v1, const Vector3f &v2);

    void sub(const Vector3f &v1);
    void sub(const Vector3f &v1, const Vector3f &v2);
    void sub(const Point3f &v1);
    void sub(const Point3f &p, const Vector3f &v);
    void sub(const Point3f &p1, const Point3f &p2);

    void scale(float s);
    void scaleAdd(float s, const Vector3f &v1, const Vector3f &v2);

    void negate();

    bool equals(const Vector3f &v1) const;
    bool epsilonEquals(const Vector3f &v1, float epsilon) const;

    friend std::ostream &operator<<(std::ostream &os, const Vector3f &o)
    {
        return os << "(" << o.x << " : " << o.y << " : " << o.z << ")";
    }
};