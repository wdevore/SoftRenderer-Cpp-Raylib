#pragma once
#include <iostream>

#include "VectorBase.h"

namespace Maths
{
    class Vector4f : public VectorBase
    {
    private:
        /* data */
    public:
        Vector4f(/* args */);
        Vector4f(float _x, float _y, float _z) : VectorBase(_x, _y, _z) {};
        ~Vector4f();

        void set(const VectorBase &c) override;
        void setFrom3(const VectorBase &c);

        void add(const VectorBase &c) override;
        void add(const VectorBase &a, const VectorBase &b) override;
        void sub(const VectorBase &c) override;
        void sub(const VectorBase &a, const VectorBase &b) override;
        void multiply(float s) override;
        void divide(float s) override;

        void print() const;
        std::string toString() const;

        friend std::ostream &operator<<(std::ostream &os, const Vector4f &v)
        {
            return os << "<" << v.x << ", " << v.y << ", " << v.z << ", " << v.w << ">";
        }
    };
} // namespace Maths
