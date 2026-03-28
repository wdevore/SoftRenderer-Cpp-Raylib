#pragma once

#include <vector>

#include "Face.h"
#include "Matrix4f.h"

// Vertices are kept in the Pipeline
class Object3D
{
public:
    enum class ObjectType
    {
        Line,
        WireMesh,
        FlatShaded
    };

private:
protected:
    Matrix4f model{};

public:
    // Each face has three vertices and a Normal
    std::vector<Face> faces;
    ObjectType type = ObjectType::Line;

    Object3D(/* args */);
    ~Object3D();

    // ========== Types ===============
    ObjectType GetType() const { return type; }

    virtual void Build(std::vector<Vector3f> &vertices) = 0;
    Matrix4f &GetModel() { return model; }

    void CalcFaceNormal(std::vector<Vector3f> &vertices, Face &face);
};
