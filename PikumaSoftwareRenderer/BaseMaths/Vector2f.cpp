#include "VectorBase.h"
#include "Vector2f.h"

namespace Maths
{
    Vector2f::Vector2f(/* args */)
    {
    }

    Vector2f::~Vector2f()
    {
    }

    void Vector2f::set(const VectorBase &c)
    {
        x = c.x;
        y = c.y;
        z = 0.0f;
        w = 0.0f;
    }

    void Vector2f::add(const VectorBase &c)
    {
        x += c.x;
        y += c.y;
    }

    void Vector2f::sub(const VectorBase &c)
    {
        x -= c.x;
        y -= c.y;
    }

    void Vector2f::multiply(float s)
    {
        x *= s;
        y *= s;
    }

    void Vector2f::divide(float s)
    {
        if (s == 0.0f)
            return;

        x /= s;
        y /= s;
    }
}