#pragma once

#include "Vector3f.h"
#include "Object3D.h"

class Tetrahedron : public Object3D
{
private:
    int o{}; // offset

public:
    Tetrahedron(/* args */);
    ~Tetrahedron();

    virtual void Build(std::vector<Vector3f> &vertices) override;
};
