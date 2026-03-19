#pragma once

#include "Vector3f.h"
#include "CColor.h"

/// @brief Transformed Vertex fragment of the pipeline
struct TransformedVertex
{
    Vector3f screenPos;
    CColor color;
    bool clipped;
};
