#include <algorithm>
#include <iostream>

#include "Object3D.h"
#include "Vertex3f.h"
#include "Triangle.h"

Object3D::Object3D()
{
}

Object3D::~Object3D()
{
}

void Object3D::reset()
{
    for (auto &v : vertices)
    {
        v.transformed = false;
    }
}

void Object3D::addVertex(Vertex3f v)
{
    vertices.push_back(v);
}

void Object3D::setPosition(float x, float y, float z)
{
    position.set(x, y, z);
}

void Object3D::setOrientation(float x, float y, float z, float angle)
{
    aa.set(x, y, z, angle);
}

/// @brief returns a matrix that transforms the object from local to
///        world-space; NOT camera or view-space.
/// @return
Matrix4f &Object3D::GetModelToWorldMatrix()
{
    rotation.setIdentity();
    rotation.setRotation(aa);

    translation.setIdentity();
    translation.setTranslation(position);

    /*
     * Note the order in which I perform the multiplication. To do a
     * rotation followed by a translation which will create an orbiting effect.
     *
     * To have a rotating effect about the objects local position
     * I would do a translation first followed by a rotation. As done below.
     */
    transform.mul(translation, rotation);

    /*
     * By calling the setRotation and setTranslation methods on
     * the SAME matrix, regardless of whether you call setRotation
     * first or second, this will create a matrix that represents
     * a translation first followed by a rotation which is in effect
     * an orbiting camera.
     *
     * This is shown below.
     *    affineTransform.setRotation(aa);
     *    affineTransform.setTranslation(position);
     *    return affineTransform;
     *
     * I personally like to control the order so I use two matrices.
     */

    return transform;
}

Matrix4f &Object3D::GetNormalTransformMatrix()
{
    return GetModelToWorldMatrix();
}

/// @brief
/// @param i1 is 1-based
/// @param i2 is 1-based
/// @param i3 is 1-based
/// @param triangles Triangle collection
void Object3D::AddTriangle(int i1, int i2, int i3)
{
    std::unique_ptr<Triangle> t(std::make_unique<Triangle>(i1 - 1, i2 - 1, i3 - 1));
    t->Initialize();

    t->SetCenter(
        vertices[i1 - 1],
        vertices[i2 - 1],
        vertices[i3 - 1]);
    t->SetNormal(
        vertices[i1 - 1],
        vertices[i2 - 1],
        vertices[i3 - 1]);
    std::cout << "Object3D::Add Triangle: " << *t << std::endl;

    triangles.push_back(std::move(t));
}
