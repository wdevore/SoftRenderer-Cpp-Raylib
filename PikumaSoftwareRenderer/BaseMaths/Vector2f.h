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
        Vector2f(float _x, float _y) : VectorBase(_x, _y, 0.0f) {};
        Vector2f(/* args */);
        ~Vector2f();

        void set(const VectorBase &c) override;

        void add(const VectorBase &c) override;
        void sub(const VectorBase &c) override;
        void add(const VectorBase &a, const VectorBase &b) override {};
        void sub(const VectorBase &a, const VectorBase &b) override {};

        void multiply(float s) override;
        void divide(float s) override;

        void print() const;
        std::string toString() const;

        friend std::ostream &operator<<(std::ostream &os, const Vector2f &v)
        {
            return os << "<" << v.x << ", " << v.y << ">";
        }
    };

    // Declare outside the class so it's visible in the Maths namespace
    // (This allows you to call it explicitly as Maths::area)
    float area(const Vector2f &a, const Vector2f &b, const Vector2f &c);

} // namespace Maths
