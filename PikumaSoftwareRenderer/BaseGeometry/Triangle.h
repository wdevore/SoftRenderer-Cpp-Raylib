#pragma once

#include <cstdint>

#include "upng.h"

#include "Vector4f.h"
#include "Vector3f.h"
#include "Texture2f.h"
#include "Texture.h"

namespace Geometry
{
    void getNormal(Maths::Vector4f vertices[3], Maths::VectorBase &normal);

    class Triangle
    {
    public:
        static const int MAX_TRIANGLES = 10000;

    private:
        /* data */
    public:
        Maths::Vector4f points[3];
        Maths::Texture2f texcoords[3]; // TextureT type
        uint32_t color;
        upng_t *texture;

        Triangle(/* args */);
        ~Triangle();

        void set(const Triangle &t);
        void set(const Maths::Vector4f points[3], const Maths::Texture2f texcoords[3],
                 uint32_t color, upng_t *texture);

        friend void getNormal(Maths::Vector4f vertices[3], Maths::VectorBase &normal);

        void print() const;
        std::string toString() const;

        friend std::ostream &operator<<(std::ostream &os, const Triangle &t)
        {
            os << "V:{" << t.points[0] << ", " << t.points[1] << ", " << t.points[2] << "}\n";
            os << "T:{" << t.texcoords[0] << ", " << t.texcoords[1] << ", " << t.texcoords[2] << "}\n";

            return os;
        }
    };

}