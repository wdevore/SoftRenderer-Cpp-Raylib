#include "PointLight.h"

PointLight::PointLight(/* args */)
{
}

PointLight::~PointLight()
{
}

Vector3f &PointLight::CalcLightRay(const Point3f &p)
{
    // Form a ray from a point(p) on the triangle to light source.
    // light position - triangle center.
    direction.sub(base, p);
    direction.normalize();
    return direction;
}
