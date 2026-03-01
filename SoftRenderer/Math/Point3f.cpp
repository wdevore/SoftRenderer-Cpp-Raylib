#include "Point3f.h"
#include <cmath>
#include <sstream>

Point3f::Point3f() : x(0.0f), y(0.0f), z(0.0f) {}

Point3f::Point3f(float x, float y, float z) : x(x), y(y), z(z) {}

Point3f::Point3f(const Point3f &p1) : x(p1.x), y(p1.y), z(p1.z) {}

void Point3f::set(float x, float y, float z)
{
    this->x = x;
    this->y = y;
    this->z = z;
}

void Point3f::set(const Point3f &p)
{
    this->x = p.x;
    this->y = p.y;
    this->z = p.z;
}

void Point3f::set(const Vector3f &v)
{
    this->x = v.x;
    this->y = v.y;
    this->z = v.z;
}

float Point3f::distance(const Point3f &p) const
{
    float dx = x - p.x;
    float dy = y - p.y;
    float dz = z - p.z;
    return std::sqrt(dx * dx + dy * dy + dz * dz);
}

float Point3f::distanceSquared(const Point3f &p) const
{
    float dx = x - p.x;
    float dy = y - p.y;
    float dz = z - p.z;
    return dx * dx + dy * dy + dz * dz;
}

void Point3f::add(const Vector3f &v)
{
    x += v.x;
    y += v.y;
    z += v.z;
}

void Point3f::sub(const Vector3f &v)
{
    x -= v.x;
    y -= v.y;
    z -= v.z;
}

void Point3f::sub(const Point3f &p1, const Point3f &p2)
{
    x = p1.x - p2.x;
    y = p1.y - p2.y;
    z = p1.z - p2.z;
}

void Point3f::scale(float s)
{
    x *= s;
    y *= s;
    z *= s;
}

std::string Point3f::toString() const
{
    std::stringstream ss;
    ss << "(" << x << ", " << y << ", " << z << ")";
    return ss.str();
}

bool Point3f::equals(const Point3f &p1) const
{
    return x == p1.x && y == p1.y && z == p1.z;
}

bool Point3f::epsilonEquals(const Point3f &p1, float epsilon) const
{
    return std::abs(p1.x - x) < epsilon &&
           std::abs(p1.y - y) < epsilon &&
           std::abs(p1.z - z) < epsilon;
}