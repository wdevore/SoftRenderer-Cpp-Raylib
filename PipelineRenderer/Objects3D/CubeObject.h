#pragma once

#include "Vector3f.h"
#include "Object3D.h"

class CubeObject : public Object3D
{
private:
    /* data */
public:
    CubeObject(/* args */);
    ~CubeObject();

    virtual void Build(std::vector<Vector3f> &vertices) override;
};
