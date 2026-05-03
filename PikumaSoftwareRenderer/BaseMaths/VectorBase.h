#pragma once

#include <iostream>

namespace Maths
{
    enum RotateAxis
    {
        X,
        Y,
        Z
    };

    using fco = float[4];

    class VectorBase
    {
    private:
        /* data */
    public:
        // Comps co{0.0f, 0.0f, 0.0f, 0.0f};
        union
        {
            fco co;
            struct
            {
                union
                {
                    float x;
                    float u;
                    float r;
                };
                union
                {
                    float y;
                    float v;
                    float g;
                };
                union
                {
                    float z;
                    float b;
                };
                union
                {
                    float w;
                    float a;
                };
            };
        };

        VectorBase(/* args */);
        // VectorBase(float _x, float _y, float _z) : x(_x), y(_y), z(_z), w(1.0) {};
        VectorBase(float _x, float _y, float _z)
        {
            x = _x;
            y = _y;
            z = _z;
            w = 1.0f;
        }
        ~VectorBase();

        virtual void set(const VectorBase &c) = 0;

        virtual void add(const VectorBase &c) = 0;
        virtual void add(const VectorBase &a, const VectorBase &b) = 0;
        virtual void sub(const VectorBase &c) = 0;
        virtual void sub(const VectorBase &a, const VectorBase &b) = 0;
        virtual void multiply(float s) = 0;
        virtual void divide(float s) = 0;

        virtual void rotateOn(float angle, RotateAxis axis) {};

        virtual void negate() {};
        virtual void zero() {};
        virtual float length() { return 0; };
        virtual float lengthSquared() { return 0; };
        virtual void normalize() {};
        virtual float dot(const VectorBase &c) { return 0; };
        virtual void cross(const VectorBase &a, const VectorBase &b) {};

        // virtual void print() const;
        // virtual std::string toString() const;

        // friend std::ostream &operator<<(std::ostream &os, const VectorBase &v)
        // {
        //     return os << "<" << v.x << ", " << v.y << ", " << v.z << ", " << v.w << ">";
        // }
    };

} // namespace Maths
