#include <sstream>

#include "VectorBase.h"

namespace Maths
{
    VectorBase::VectorBase(/* args */)
    {
        co[0] = 0.0f;
        co[1] = 0.0f;
        co[2] = 0.0f;
        co[3] = 0.0f;
    }

    VectorBase::~VectorBase()
    {
    }

    // void VectorBase::print() const
    // {
    //     std::cout << *this << std::endl;
    // }

    // std::string VectorBase::toString() const
    // {
    //     std::ostringstream oss;
    //     oss << *this;

    //     return oss.str();
    // }

}