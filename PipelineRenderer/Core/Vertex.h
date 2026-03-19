#pragma once

#include "Vector3f.h"
#include "CColor.h"

/// @brief Vertex fragment of the pipeline
struct Vertex
{
    Vector3f position{};
    CColor color{};
};
