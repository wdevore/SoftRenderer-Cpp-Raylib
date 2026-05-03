#include <cmath>
#include <sstream>

#include "VectorBase.h"
#include "Vector4f.h"

namespace Maths
{
    Vector4f::Vector4f(/* args */)
    {
    }

    Vector4f::~Vector4f()
    {
    }

    void Vector4f::set(const VectorBase &c)
    {
        x = c.x;
        y = c.y;
        z = c.z;
        w = c.w;
    }

    void Vector4f::setFrom3(const VectorBase &c)
    {
        x = c.x;
        y = c.y;
        z = c.z;
        w = 1.0f;
    }

    void Vector4f::sub(const VectorBase &a, const VectorBase &b)
    {
    }

    void Vector4f::add(const VectorBase &c) {}

    void Vector4f::add(const VectorBase &a, const VectorBase &b)
    {
    }

    void Vector4f::sub(const VectorBase &c)
    {
    }

    void Vector4f::multiply(float s)
    {
    }

    void Vector4f::divide(float s)
    {
    }

    void Vector4f::print() const
    {
        std::cout << *this << std::endl;
    }

    std::string Vector4f::toString() const
    {
        std::ostringstream oss;
        oss << *this;

        return oss.str();
    }

}