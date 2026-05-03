#pragma once

#include <iostream>

#include "VectorBase.h"

namespace Maths
{
    class Vector3f : public VectorBase
    {
    private:
        /* data */
    public:
        Vector3f(/* args */) : VectorBase() {};
        Vector3f(float _x, float _y, float _z) : VectorBase(_x, _y, _z) {};
        Vector3f(const Vector3f &c) : VectorBase(c) {};
        ~Vector3f();

        void set(const VectorBase &c) override;
        void set(float x, float y, float z);

        void zero() override;
        void add(const VectorBase &c) override;
        void add(const VectorBase &a, const VectorBase &b) override;
        void sub(const VectorBase &c) override;
        void sub(const VectorBase &a, const VectorBase &b) override;
        void multiply(float s) override;
        void divide(float s) override;
        void normalize() override;

        void rotateOn(float angle, RotateAxis axis) override;

        void cross(const VectorBase &a, const VectorBase &b) override;
        float dot(const VectorBase &c) override;

        void print() const;
        std::string toString() const;

        friend std::ostream &operator<<(std::ostream &os, const Vector3f &v)
        {
            return os << "<" << v.x << ", " << v.y << ", " << v.z << ">";
        }
    };
} // namespace Maths
