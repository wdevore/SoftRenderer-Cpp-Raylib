#include "Object3D.h"

Object3D::Object3D(/* args */)
{
}

Object3D::~Object3D()
{
}

void Object3D::CalcFaceNormal(std::vector<Vector3f> &vertices, Face &face)
{
    Vector3f &v1 = vertices[face.i1];
    Vector3f &v2 = vertices[face.i2];
    Vector3f &v3 = vertices[face.i3];

    Vector3f tv1{};
    Vector3f tv2{};
    Vector3f tv3{};

    tv1.sub(v2, v1);             // v2 - v1
    tv2.sub(v3, v1);             // v3 - v1
    face.normal.cross(tv1, tv2); // normal vector non-normalized (tv1 into tv2)
    face.normal.normalize();
}
