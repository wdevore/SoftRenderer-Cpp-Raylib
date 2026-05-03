#include <sstream>

#include "VectorBase.h"
#include "Texture2f.h"

namespace Maths
{
    Texture2f::Texture2f(/* args */)
    {
    }

    Texture2f::~Texture2f()
    {
    }

    void Texture2f::set(const VectorBase &c)
    {
        u = c.u;
        v = c.v;
        z = 0.0f;
        w = 0.0f;
    }

    void Texture2f::sub(const VectorBase &a, const VectorBase &b)
    {
    }

    void Texture2f::add(const VectorBase &c)
    {
    }

    void Texture2f::add(const VectorBase &a, const VectorBase &b)
    {
    }

    void Texture2f::sub(const VectorBase &c)
    {
    }

    void Texture2f::multiply(float s)
    {
    }

    void Texture2f::divide(float s)
    {
    }

    void Texture2f::print() const
    {
        std::cout << *this << std::endl;
    }

    std::string Texture2f::toString() const
    {
        std::ostringstream oss;
        oss << *this;

        return oss.str();
    }

}