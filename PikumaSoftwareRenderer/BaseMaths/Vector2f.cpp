#include "Vector2f.h"
#include <sstream>

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

    void Vector2f::print() const
    {
        std::cout << *this << std::endl;
    }

    std::string Vector2f::toString() const
    {
        std::ostringstream oss;
        oss << *this;

        return oss.str();
    }

    /// @brief 2D edge cross product
    /// @param a
    /// @param b
    /// @param p
    /// @return
    float area(const Vector2f &a, const Vector2f &b, const Vector2f &p)
    {
        Vector2f ab = {b.x - a.x, b.y - a.y};
        Vector2f ap = {p.x - a.x, p.y - a.y};
        return ab.x * ap.y - ab.y * ap.x;

        // return (b.x - a.x) * (p.y - a.y) - (b.y - a.y) * (p.x - a.x);
    }
}