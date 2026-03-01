#pragma once

#include <vector>
#include <string>

#include "Matrix4f.h"
#include "Vertex3f.h"

class Object3D
{
public:
    Object3D();
    virtual ~Object3D() = 0; // Makes class abstract

    std::string name{};
    bool animate{};

    std::vector<Vertex3f> vertices{};
    std::vector<Vertex3f> vertex_normals{};

    int r{};
    int g{};
    int b{};
    int a{};

    void reset();
    void addVertex(Vertex3f v);
    void setColor(int r, int g, int b, int a = 255);

    void setPosition(float x, float y, float z);
    void setOrientation(float x, float y, float z, float angle);

    Matrix4f &getModelToWorldMatrix();
    int GetVertexCount() { return vertices.size(); }

    virtual void SetAnimate(bool animate) = 0;

protected:
    Vector3f p1{};
    Vector3f p2{};
    Vector3f p3{};

private:
    Matrix4f translation{};
    Matrix4f rotation{};
    Vector3f position{};
    AxisAngle4f aa{};

    // The combined rotation and translation
    Matrix4f transform{};
};