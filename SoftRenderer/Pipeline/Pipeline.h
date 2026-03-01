#pragma once

#include "Frustum.h"

// Pipeline takes a vector of vertices, edges and normals.
//
// The pipeline performs several sequences before final rasterization in pixel
// space. First it transforms all vertices to view-space, then
// First is transforms all vertices through different spaces until it reaches
// pixel space.

class Pipeline
{
private:
    // Frustum representing viewing volume
    Frustum frustum{};

public:
    Pipeline(/* args */);
    ~Pipeline();
};
