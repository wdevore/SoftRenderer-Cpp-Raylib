#pragma once

#include <iostream>

#include "VectorBase.h"

namespace Maths
{
    class Vector2f : public VectorBase
    {
    private:
        /* data */
    public:
        Vector2f(/* args */);
        ~Vector2f();

        void set(const VectorBase &c) override;

        void add(const VectorBase &c) override;
        void sub(const VectorBase &c) override;
        void multiply(float s) override;
        void divide(float s) override;

        friend std::ostream &operator<<(std::ostream &os, const Vector2f &v)
        {
            return os << "<" << v.x << ", " << v.y << ">";
        }
    };
} // namespace Maths
