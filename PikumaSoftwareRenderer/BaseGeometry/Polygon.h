#pragma once
#include <iostream>
#include <vector>

#include "Vector3f.h"
#include "Texture2f.h"
#include "Triangle.h"

namespace Geometry
{
    class Polygon
    {
    private:
    public:
        static const int MAX_VERTICES = 10;
        static const int MAX_NUM_POLY_TRIANGLES = 10;

        std::vector<Maths::Vector3f> vertices{};
        std::vector<Maths::Texture2f> texcoords{};

        int numVertices{};

        Polygon(/* args */);
        ~Polygon();

        /// @brief
        /// @param v0 Vertex
        /// @param v1
        /// @param v2
        /// @param t0 Texture coordinate
        /// @param t1
        /// @param t2
        void setFromTriangle(Maths::Vector4f &v0, Maths::Vector4f &v1, Maths::Vector4f &v2,
                             Maths::Texture2f &t0, Maths::Texture2f &t1, Maths::Texture2f &t2);

        void print() const;
        std::string toString() const;

        friend std::ostream &operator<<(std::ostream &os, const Polygon &v)
        {
            return os << "vertices: " << v.vertices.size() << ", texcoords: " << v.texcoords.size() << ", numVertices: " << v.numVertices << std::endl;
        }
    };

    void trianglesFromPolygon(Polygon &polygon, std::vector<Geometry::Triangle> &triangles, int &numTriangles);

} // namespace Geometry
