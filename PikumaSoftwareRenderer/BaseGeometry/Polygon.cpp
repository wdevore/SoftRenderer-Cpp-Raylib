#include <sstream>

#include "Polygon.h"

namespace Geometry
{
    Polygon::Polygon(/* args */)
    {
        vertices.resize(MAX_VERTICES);
        texcoords.resize(MAX_VERTICES);
    }

    Polygon::~Polygon()
    {
    }

    void Polygon::setFromTriangle(Maths::Vector4f &v0, Maths::Vector4f &v1, Maths::Vector4f &v2,
                                  Maths::Texture2f &t0, Maths::Texture2f &t1, Maths::Texture2f &t2)
    {
        numVertices = 3;

        vertices[0].set(v0);
        vertices[1].set(v1);
        vertices[2].set(v2);

        texcoords[0].set(t0);
        texcoords[1].set(t1);
        texcoords[2].set(t2);
    }

    void trianglesFromPolygon(Polygon &polygon, std::vector<Geometry::Triangle> &triangles, int &numTriangles)
    {
        numTriangles = polygon.numVertices - 2;

        for (int i = 0; i < numTriangles; i++)
        {
            int index0 = 0;
            int index1 = i + 1;
            int index2 = i + 2;

            // Convert from VectorT3 to VectorT4
            triangles[i].points[0].setFrom3(polygon.vertices[index0]);
            triangles[i].points[1].setFrom3(polygon.vertices[index1]);
            triangles[i].points[2].setFrom3(polygon.vertices[index2]);

            triangles[i].texcoords[0].set(polygon.texcoords[index0]);
            triangles[i].texcoords[1].set(polygon.texcoords[index1]);
            triangles[i].texcoords[2].set(polygon.texcoords[index2]);
        }
    }

    void Polygon::print() const
    {
        std::cout << *this << std::endl;
    }

    std::string Polygon::toString() const
    {
        std::ostringstream oss;
        oss << *this;

        return oss.str();
    }

} // namespace Geometry
