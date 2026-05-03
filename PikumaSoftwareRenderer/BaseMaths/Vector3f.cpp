#include <cmath>
#include <sstream>

#include "VectorBase.h"
#include "Vector3f.h"

namespace Maths
{
    Vector3f::~Vector3f()
    {
    }

    void Vector3f::set(const VectorBase &c)
    {
        x = c.x;
        y = c.y;
        z = c.z;
        w = 1.0f;
    }

    void Vector3f::set(float x, float y, float z)
    {
        this->x = x;
        this->y = y;
        this->z = z;
        w = 1.0f;
    }

    void Vector3f::zero()
    {
        x = 0.0f;
        y = 0.0f;
        z = 0.0f;
        w = 1.0f;
    }

    void Vector3f::add(const VectorBase &c)
    {
        x += c.x;
        y += c.y;
        z += c.z;
    }

    void Vector3f::add(const VectorBase &a, const VectorBase &b)
    {
        x = a.x + b.x;
        y = a.y + b.y;
        z = a.z + b.z;
    }

    void Vector3f::sub(const VectorBase &c)
    {
        x -= c.x;
        y -= c.y;
        z -= c.z;
    }

    void Vector3f::sub(const VectorBase &a, const VectorBase &b)
    {
        x = a.x - b.x;
        y = a.y - b.y;
        z = a.z - b.z;
    }

    void Vector3f::multiply(float s)
    {
        x *= s;
        y *= s;
        z *= s;
    }

    void Vector3f::divide(float s)
    {
        if (s == 0.0f)
            return;

        x /= s;
        y /= s;
        z /= s;
    }

    void Vector3f::normalize()
    {
        float length = std::sqrt(x * x + y * y + z * z);
        if (length != 0.0f)
        {
            x /= length;
            y /= length;
            z /= length;
        }
    }

    void Vector3f::rotateOn(float angle, RotateAxis axis)
    {
        float s = std::sin(angle);
        float c = std::cos(angle);
        float x = x;
        float y = y;
        float z = z;

        switch (axis)
        {
        case X:
            y = y * c - z * s;
            z = y * s + z * c;
            break;
        case Y:
            x = x * c + z * s;
            z = x * s + z * c;
            break;
        case Z:
            x = x * c - y * s;
            y = x * s + y * c;
            break;
        }
    }

    void Vector3f::cross(const VectorBase &a, const VectorBase &b)
    {
        x = a.y * b.z - a.z * b.y;
        y = a.z * b.x - a.x * b.z;
        z = a.x * b.y - a.y * b.x;
    }

    float Vector3f::dot(const VectorBase &c)
    {
        return x * c.x +
               y * c.y +
               z * c.z;
    }

    void Vector3f::print() const
    {
        std::cout << *this << std::endl;
    }

    std::string Vector3f::toString() const
    {
        std::ostringstream oss;
        oss << *this;

        return oss.str();
    }
}