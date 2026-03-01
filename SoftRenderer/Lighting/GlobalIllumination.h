#pragma once

#include "Vector3f.h"

class GlobalIllumination
{
private:
    /* data */
public:
    float Kd{0.6f};    // diffuse
    float Ia{0.3f};    // ambient
    float Ks{1000.0f}; // specular
    float KdIa{};      // background diffuse + ambient

    GlobalIllumination(/* args */);
    ~GlobalIllumination();

    float CalcPartialLambertIntensity(float angle, float Ip);
    float CalcPartialLambertIntensity(const Vector3f &normal, const Vector3f &lightRay, float Ip);
    float CalcIntensity(float partialIntensity);
};
