#include "Vector3f.h"
#include <cmath>
#include <sstream>

Vector3f::Vector3f() : x(0.0f), y(0.0f), z(0.0f) {}

Vector3f::Vector3f(float x, float y, float z) : x(x), y(y), z(z) {}

Vector3f::Vector3f(const Vector3f &v1) : x(v1.x), y(v1.y), z(v1.z) {}

void Vector3f::set(float x, float y, float z)
{
    this->x = x;
    this->y = y;
    this->z = z;
}

void Vector3f::set(const Vector3f &v1)
{
    this->x = v1.x;
    this->y = v1.y;
    this->z = v1.z;
}

float Vector3f::length() const
{
    return std::sqrt(x * x + y * y + z * z);
}

float Vector3f::lengthSquared() const
{
    return x * x + y * y + z * z;
}

void Vector3f::normalize()
{
    float l = length();
    if (l != 0.0f)
    {
        x /= l;
        y /= l;
        z /= l;
    }
}

float Vector3f::dot(const Vector3f &v1) const
{
    return x * v1.x + y * v1.y + z * v1.z;
}

void Vector3f::cross(const Vector3f &v1, const Vector3f &v2)
{
    float cx = v1.y * v2.z - v1.z * v2.y;
    float cy = v1.z * v2.x - v1.x * v2.z;
    float cz = v1.x * v2.y - v1.y * v2.x;
    this->x = cx;
    this->y = cy;
    this->z = cz;
}

void Vector3f::add(const Vector3f &v1)
{
    x += v1.x;
    y += v1.y;
    z += v1.z;
}

void Vector3f::add(const Vector3f &v1, const Vector3f &v2)
{
    x = v1.x + v2.x;
    y = v1.y + v2.y;
    z = v1.z + v2.z;
}

void Vector3f::sub(const Vector3f &v1)
{
    x -= v1.x;
    y -= v1.y;
    z -= v1.z;
}

void Vector3f::sub(const Vector3f &v1, const Vector3f &v2)
{
    x = v1.x - v2.x;
    y = v1.y - v2.y;
    z = v1.z - v2.z;
}

void Vector3f::scale(float s)
{
    x *= s;
    y *= s;
    z *= s;
}

void Vector3f::scaleAdd(float s, const Vector3f &v1, const Vector3f &v2)
{
    x = s * v1.x + v2.x;
    y = s * v1.y + v2.y;
    z = s * v1.z + v2.z;
}

std::string Vector3f::toString() const
{
    std::stringstream ss;
    ss << "(" << x << ", " << y << ", " << z << ")";
    return ss.str();
}

bool Vector3f::equals(const Vector3f &v1) const
{
    return x == v1.x && y == v1.y && z == v1.z;
}

bool Vector3f::epsilonEquals(const Vector3f &v1, float epsilon) const
{
    return std::abs(v1.x - x) < epsilon &&
           std::abs(v1.y - y) < epsilon &&
           std::abs(v1.z - z) < epsilon;
}