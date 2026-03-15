#pragma once

#include "LightBase.h"

class PointLight : public LightBase
{
private:
    /* data */
public:
    PointLight(/* args */);
    ~PointLight();

    Vector3f &CalcLightRay(const Point3f &p) override;
    Vector3f &CalcLightRay() override
    {
        direction.set(0.0f, 0.0f, 0.0f);
        return direction;
    }
};
