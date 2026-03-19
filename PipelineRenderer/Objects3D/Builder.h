#pragma once

#include <vector>

#include "Vector3f.h"
#include "CubeObject.h"

// ============ Builders ==================

class Builder
{
private:
    std::vector<Vector3f> vertices{};

public:
    Builder(/* args */);
    ~Builder();

    // ============ Builders ==================
    // Builders accept a 3D object and return Vector3f collection reference.
    std::vector<Vector3f> &BuildCube(CubeObject &cube);
};
