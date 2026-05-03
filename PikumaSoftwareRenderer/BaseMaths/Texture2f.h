#pragma once

#include <cstdint>
#include <iostream>

#include "upng.h"

#include "VectorBase.h"

namespace Maths
{
    class Texture2f : public VectorBase
    {
    private:
        /* data */
    public:
        uint32_t color;
        upng_t *image;

        Texture2f(/* args */);
        Texture2f(float _u, float _v) : VectorBase(_u, _v, 0.0f) {};
        Texture2f(const Texture2f &c) : VectorBase(c) {};
        ~Texture2f();

        void set(const VectorBase &c) override;

        void add(const VectorBase &c) override;
        void add(const VectorBase &a, const VectorBase &b) override;
        void sub(const VectorBase &a, const VectorBase &b) override;
        void sub(const VectorBase &c) override;
        void multiply(float s) override;
        void divide(float s) override;

        void print() const;
        std::string toString() const;

        friend std::ostream &operator<<(std::ostream &os, const Texture2f &t)
        {
            return os << "<" << t.u << ", " << t.v << ">";
        }
    };
} // namespace Maths
