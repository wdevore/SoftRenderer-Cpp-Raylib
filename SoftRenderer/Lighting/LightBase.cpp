#include "LightBase.h"

LightBase::LightBase(/* args */)
{
}

LightBase::~LightBase()
{
}

void LightBase::SetDirection(float bx, float by, float bz, float tx, float ty, float tz)
{
    base.set(bx, by, bz);
    target.set(tx, ty, tz);
    direction.sub(target, base);
    direction.normalize();
}

void LightBase::SetDirection(float bx, float by, float bz)
{
    direction.set(bx, by, bz);
    direction.normalize();
}

void LightBase::SetColor(int r, int g, int b)
{
    this->r = r;
    this->g = g;
    this->b = b;
}
