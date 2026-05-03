#include <cmath>

#include "Frustum.h"
#include "Vector3f.h"
#include "Utilities.h"

namespace Geometry
{
    Frustum::Frustum(/* args */)
    {
        insideVertices.resize(Geometry::Polygon::MAX_VERTICES);
        insideTexcoords.resize(Geometry::Polygon::MAX_VERTICES);
    }

    Frustum::~Frustum()
    {
    }

    void Frustum::initialize(float fovX, float fovY, float znear, float zfar)
    {
        float cos_half_fov_x = std::cos(fovX / 2);
        float sin_half_fov_x = std::sin(fovX / 2);
        float cos_half_fov_y = std::cos(fovY / 2);
        float sin_half_fov_y = std::sin(fovY / 2);

        planes.resize(6);

        planes[LEFT].point = Maths::Vector3f{};
        planes[LEFT].normal.x = cos_half_fov_x;
        planes[LEFT].normal.y = 0;
        planes[LEFT].normal.z = sin_half_fov_x;

        planes[RIGHT].point = Maths::Vector3f{};
        planes[RIGHT].normal.x = -cos_half_fov_x;
        planes[RIGHT].normal.y = 0;
        planes[RIGHT].normal.z = sin_half_fov_x;

        planes[TOP].point = Maths::Vector3f{};
        planes[TOP].normal.x = 0;
        planes[TOP].normal.y = -cos_half_fov_y;
        planes[TOP].normal.z = sin_half_fov_y;

        planes[BOTTOM].point = Maths::Vector3f{};
        planes[BOTTOM].normal.x = 0;
        planes[BOTTOM].normal.y = cos_half_fov_y;
        planes[BOTTOM].normal.z = sin_half_fov_y;

        planes[NEAR].point = Maths::Vector3f{0, 0, znear};
        planes[NEAR].normal.x = 0;
        planes[NEAR].normal.y = 0;
        planes[NEAR].normal.z = 1;

        planes[FAR].point = Maths::Vector3f{0, 0, zfar};
        planes[FAR].normal.x = 0;
        planes[FAR].normal.y = 0;
        planes[FAR].normal.z = -1;
    }

    void Frustum::clip(Polygon &polygon)
    {
        clipAgainstPlane(polygon, LEFT);
        clipAgainstPlane(polygon, RIGHT);
        clipAgainstPlane(polygon, TOP);
        clipAgainstPlane(polygon, BOTTOM);
        clipAgainstPlane(polygon, NEAR);
        clipAgainstPlane(polygon, FAR);
    }

    void Frustum::clipAgainstPlane(Polygon &polygon, FrustumPlane plane)
    {
        Maths::Vector3f *planePoint = &planes[plane].point;
        Maths::Vector3f *planeNormal = &planes[plane].normal;

        // Declare a static array of inside vertices that will be part of the final polygon returned via parameter
        int numInsideVertices = 0;

        // Start the current vertex with the first polygon vertex and texture coordinate
        Maths::Vector3f *currentVertex = &polygon.vertices[0];
        Maths::Texture2f *currentTexcoord = &polygon.texcoords[0];

        // Start the previous vertex with the last polygon vertex and texture coordinate
        Maths::Vector3f *previousVertex = &polygon.vertices[polygon.numVertices - 1];
        Maths::Texture2f *previousTexcoord = &polygon.texcoords[polygon.numVertices - 1];

        // Calculate the dot product of the current and previous vertex
        float currentDot = 0;
        Maths::Vector3f ray{};
        ray.sub(*previousVertex, *planePoint);

        float previousDot = ray.dot(*planeNormal);

        // Loop all the polygon vertices while the current is different than the last one
        while (currentVertex != &polygon.vertices[polygon.numVertices])
        {
            ray.sub(*currentVertex, *planePoint);
            currentDot = ray.dot(*planeNormal);

            // If we changed from inside to outside or from outside to inside
            if (currentDot * previousDot < 0)
            {
                // Find the interpolation factor t
                float t = previousDot / (previousDot - currentDot);

                // Calculate the intersection point I = Q1 + t(Q2-Q1)
                Maths::Vector3f intersectionPoint{
                    Utilities::floatLerp(previousVertex->x, currentVertex->x, t),
                    Utilities::floatLerp(previousVertex->y, currentVertex->y, t),
                    Utilities::floatLerp(previousVertex->z, currentVertex->z, t)};

                // Use the lerp formula to get the interpolated U and V texture coordinates
                Maths::Texture2f interpolatedTexcoord{
                    Utilities::floatLerp(previousVertex->u, currentVertex->u, t),
                    Utilities::floatLerp(previousVertex->v, currentVertex->v, t)};

                // Insert the intersection point to the list of "inside vertices"
                insideVertices[numInsideVertices] = intersectionPoint;
                insideTexcoords[numInsideVertices] = interpolatedTexcoord;
                numInsideVertices++;
            }

            // Current vertex is inside the plane
            if (currentDot > 0)
            {
                // Insert the current vertex to the list of "inside vertices"
                insideVertices[numInsideVertices] = Maths::Vector3f{*currentVertex};
                insideTexcoords[numInsideVertices] = Maths::Texture2f{*currentTexcoord};
                numInsideVertices++;
            }

            // Move to the next vertex
            previousDot = currentDot;
            previousVertex = currentVertex;
            previousTexcoord = currentTexcoord;
            currentVertex++;
            currentTexcoord++;
        }

        // At the end, copy the list of inside vertices into the destination polygon (out parameter)
        for (int i = 0; i < numInsideVertices; i++)
        {
            polygon.vertices[i] = Maths::Vector3f{insideVertices[i]};
            polygon.texcoords[i] = Maths::Texture2f{insideTexcoords[i]};
        }
        polygon.numVertices = numInsideVertices;
    }

} // namespace Geometry
