#include "AxisAngle4f.h"
#include <cmath>
#include <sstream>

AxisAngle4f::AxisAngle4f() : x(0.0f), y(0.0f), z(1.0f), angle(0.0f)
{
}

AxisAngle4f::AxisAngle4f(float x, float y, float z, float angle) : x(x), y(y), z(z), angle(angle)
{
}

AxisAngle4f::AxisAngle4f(const AxisAngle4f &a1) : x(a1.x), y(a1.y), z(a1.z), angle(a1.angle)
{
}

void AxisAngle4f::set(float x, float y, float z, float angle)
{
    this->x = x;
    this->y = y;
    this->z = z;
    this->angle = angle;
}

void AxisAngle4f::set(const AxisAngle4f &a1)
{
    this->x = a1.x;
    this->y = a1.y;
    this->z = a1.z;
    this->angle = a1.angle;
}

std::string AxisAngle4f::toString() const
{
    std::stringstream ss;
    ss << "(" << x << ", " << y << ", " << z << ", " << angle << ")";
    return ss.str();
}

bool AxisAngle4f::equals(const AxisAngle4f &a1) const
{
    return x == a1.x && y == a1.y && z == a1.z && angle == a1.angle;
}

bool AxisAngle4f::epsilonEquals(const AxisAngle4f &a1, float epsilon) const
{
    return std::abs(a1.x - x) < epsilon &&
           std::abs(a1.y - y) < epsilon &&
           std::abs(a1.z - z) < epsilon &&
           std::abs(a1.angle - angle) < epsilon;
}

bool AxisAngle4f::operator==(const AxisAngle4f &other) const
{
    return equals(other);
}

bool AxisAngle4f::operator!=(const AxisAngle4f &other) const
{
    return !equals(other);
}