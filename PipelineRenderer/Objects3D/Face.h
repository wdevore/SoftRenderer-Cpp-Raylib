#pragma once

#include "Vector3f.h"

struct Face
{
    Vector3f indices[3] = {Vector3f{}, Vector3f{}, Vector3f{}};
    Vector3f normal{};
};
