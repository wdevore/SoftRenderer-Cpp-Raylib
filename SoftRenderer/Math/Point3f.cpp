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

void Point3f::set(const Point3f &p1)
{
    this->x = p1.x;
    this->y = p1.y;
    this->z = p1.z;
}

float Point3f::distance(const Point3f &p1) const
{
    float dx = x - p1.x;
    float dy = y - p1.y;
    float dz = z - p1.z;
    return std::sqrt(dx * dx + dy * dy + dz * dz);
}

float Point3f::distanceSquared(const Point3f &p1) const
{
    float dx = x - p1.x;
    float dy = y - p1.y;
    float dz = z - p1.z;
    return dx * dx + dy * dy + dz * dz;
}

void Point3f::add(const Vector3f &v1)
{
    x += v1.x;
    y += v1.y;
    z += v1.z;
}

void Point3f::sub(const Vector3f &v1)
{
    x -= v1.x;
    y -= v1.y;
    z -= v1.z;
}

void Point3f::sub(const Point3f &p1, const Point3f &p2)
{
    x = p1.x - p2.x;
    y = p1.y - p2.y;
    z = p1.z - p2.z;
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