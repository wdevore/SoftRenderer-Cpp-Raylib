#pragma once

#include "Vector3f.h"
#include "Object3D.h"

class LineObject : public Object3D
{
private:
    int o{}; // offset

public:
    LineObject(/* args */);
    ~LineObject();

    virtual void Build(std::vector<Vector3f> &vertices) override;
};
