#include "GlobalIllumination.h"

GlobalIllumination::GlobalIllumination(/* args */)
{
    KdIa = Kd * Ia;
}

GlobalIllumination::~GlobalIllumination()
{
}

float GlobalIllumination::CalcPartialLambertIntensity(float angle, float Ip)
{
    float I = Kd * Ip * angle; // partial lambert Lambert equation.
    return I;
}

float GlobalIllumination::CalcPartialLambertIntensity(const Vector3f &normal, const Vector3f &lightRay, float Ip)
{
    float NL = normal.dot(lightRay);
    if (NL < 0.0f)
        NL = 0.0f; // is face not facing light?
    return CalcPartialLambertIntensity(NL, Ip);
}

float GlobalIllumination::CalcIntensity(float partialIntensity)
{
    float i = KdIa + partialIntensity;
    // clamp intensity to 1.0f
    i = std::min<float>(i, 1.0f);
    return i;
}
