#pragma once

#include "Vector4f.h"
#include "CColor.h"
#include "Vector3f.h"

namespace Geometry
{
    class Line
    {
    public:
        // Max numbers of lines generated from clipping
        static const int MAX_LINES = 1000;

    private:
    public:
        Maths::Vector4f points[2];
        CColor color;

        Maths::Vector3f scale{};       // scale in x, y, and z
        Maths::Vector3f rotation{};    // rotation in x, y, and z
        Maths::Vector3f translation{}; // translation in x, y, and z

        Line(/* args */);
        Line(const Maths::Vector4f &p1, const Maths::Vector4f &p2, CColor color);
        ~Line();

        void set(const Line &line);
        void set(const Maths::Vector4f &p1, const Maths::Vector4f &p2, CColor color);

        void print() const;
        std::string toString() const;

        friend std::ostream &operator<<(std::ostream &os, const Line &l)
        {
            os << "L:{" << l.points[0] << ", " << l.points[1] << "}\n";

            return os;
        }
    };
}
