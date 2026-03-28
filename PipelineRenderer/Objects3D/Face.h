#pragma once

#include "Vector3f.h"

struct Face
{
    int i1;
    int i2;
    int i3;
    Vector3f normal{};

    void Set(int i1, int i2, int i3)
    {
        this->i1 = i1;
        this->i2 = i2;
        this->i3 = i3;
    }
};
