#pragma once

#include <vector>

#include "Face.h"

// Vertices are kept in the Pipeline
class Object3D
{
private:
    // Each face has three vertices and a Normal
    std::vector<Face> faces{};

public:
    Object3D(/* args */);
    ~Object3D();

    virtual void Build(std::vector<Vector3f> &vertices) = 0;
};
