#pragma once

#include "LightBase.h"

class DirectionalLight : public LightBase
{
private:
    /* data */
public:
    DirectionalLight(/* args */);
    ~DirectionalLight();

    Vector3f &CalcLightRay(const Point3f &p) override
    {
        direction.set(0.0f, 0.0f, 0.0f);
        return direction;
    };
    Vector3f &CalcLightRay() override
    {
        return direction;
    }
};
