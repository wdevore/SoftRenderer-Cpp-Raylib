#include <sstream>

#include "Triangle.h"
#include "Vector3f.h"

namespace Geometry
{
    Triangle::Triangle(/* args */)
    {
        // for (int i = 0; i < 3; i++)
        //     texcoords[i].type = VectorType::TextureT;
    }

    Triangle::~Triangle()
    {
    }

    void Triangle::set(const Triangle &t)
    {
        for (int i = 0; i < 3; i++)
        {
            points[i].set(t.points[i]);
            texcoords[i].set(t.texcoords[i]);
        }
        color = t.color;
        texture = t.texture;
    }

    void Triangle::set(const Maths::Vector4f points[3], const Maths::Texture2f texcoords[3], uint32_t color, upng_t *texture)
    {
        for (int i = 0; i < 3; i++)
        {
            this->points[i].set(points[i]);
            this->texcoords[i].set(texcoords[i]);
        }
        this->color = color;
        this->texture = texture;
    }

    void getNormal(Maths::Vector4f vertices[3], Maths::VectorBase &normal)
    {
        // TODO: replace locals

        // Get individual vectors from A, B, and C vertices to compute normal
        Maths::Vector3f vector_a{};
        Maths::Vector3f vector_b{};
        Maths::Vector3f vector_c{};
        vector_a.set(vertices[0]); /*   A   */ // CW order
        vector_b.set(vertices[1]);             /*  / \  */
        vector_c.set(vertices[2]);             /* C---B */

        // Get the vector subtraction of B-A and C-A
        Maths::Vector3f vector_ab;
        vector_ab.sub(vector_b, vector_a);
        Maths::Vector3f vector_ac;
        vector_ac.sub(vector_c, vector_a);

        vector_ab.normalize();
        vector_ac.normalize();

        // Compute the face normal (using cross product to find perpendicular)
        normal.cross(vector_ab, vector_ac);
        normal.normalize();
    }

    void Triangle::print() const
    {
        std::cout << *this << std::endl;
    }

    std::string Triangle::toString() const
    {
        std::ostringstream oss;
        oss << *this;

        return oss.str();
    }

} // namespace Geometry