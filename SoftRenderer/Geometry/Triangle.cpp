#include "Triangle.h"

Triangle::Triangle(/* args */)
{
}

Triangle::Triangle(int i1, int i2, int i3)
{
    this->i1 = i1;
    this->i2 = i2;
    this->i3 = i3;
}

Triangle::~Triangle()
{
}

void Triangle::Initialize()
{
    edges.resize(3);
}

/// @brief
/// @param edge  is 1-based
/// @param index is 1-based
void Triangle::SetEdgeIndex(int edge, int index)
{
    edges[edge - 1] = index - 1;
}

/// @brief points are given in CCW order.
/// @param p1
/// @param p2
/// @param p3
/// @return a normalized normal vector
Vector3f &Triangle::CalcNormal(const Vertex3f &p1, const Vertex3f &p2, const Vertex3f &p3)
{
    tv1.set(CalcNonNormalized(p1, p2, p3));
    tv1.normalize();
    return tv1;
}

Vector3f &Triangle::CalcNonNormalized(const Vertex3f &p1, const Vertex3f &p2, const Vertex3f &p3)
{
    tv1.sub(p2, p1);     // p2 - p1
    tv2.sub(p3, p1);     // p3 - p1
    tv3.cross(tv1, tv2); // normal vector non-normalized (tv1 into tv2)
    return tv3;
}

/// @brief
/// @param v is either non-normal or normalized
void Triangle::SetNormal(Vector3f v)
{
    normal.set(v);
}

/// @brief Set the normal and normalizes it.
/// @param p1
/// @param p2
/// @param p3
void Triangle::SetNormal(Vertex3f p1, Vertex3f p2, Vertex3f p3)
{
    normal.set(CalcNormal(p1, p2, p3));
}

void Triangle::SetCenter(Vertex3f p1, Vertex3f p2, Vertex3f p3)
{
    // get center of triangle
    tv1.set(p1.x + p2.x, p1.y + p2.y, p1.z + p2.z);
    tv1.scale(1.0f / 2.0f);
    tv2.set(p1.x + p3.x, p1.y + p3.y, p1.z + p3.z);
    tv2.scale(1.0f / 2.0f);
    center.set(tv1.x + tv2.x, tv1.y + tv2.y, tv1.z + tv2.z);
    center.scale(1.0f / 2.0f);
}
