#pragma once

#include "Point3f.h"
#include "Vector3f.h"

// There are two kinds of lights that I support:
// 1) Point light source. Some call it an Omni-directional light.
// 2) Directional.
//
// I think of a Point light source as a source that cast
// light rays in all directions and is typically not
// that far from its targets.
//
// A direction light source would be something that is
// extremely far away and thus creates light rays that
// are coming from a fixed direction. A directional light
// only requires one thing and that is a "direction".
// Note: This direction describes where the light is coming.
// For example, a direction of 0,0,-1 means that light is
// coming from -Z axis and heading towards the +z axis so
// faces would be brighter when they are facing the -z axis
// and darkest when facing the +z axis.
//
// A point light requires a location( of the light) and a
// target(where the light is shining onto). This allows a
// lightRay to be astablished for a particular point in space. The
// target chosen is always the center of the triangle.

class LightBase
{
private:
    /* data */

protected:
    Point3f base{};
    Point3f target{};
    Vector3f direction{};

    int r{};
    int g{};
    int b{};

public:
    float Ip{1.0f};

    LightBase(/* args */);
    virtual ~LightBase() = 0; // Makes class abstract

    void SetDirection(float bx, float by, float bz, float tx, float ty, float tz);
    void SetDirection(float bx, float by, float bz);
    void SetColor(int r, int g, int b);
    void SetBase(float x, float y, float z) { base.set(x, y, z); };

    int R() { return r; };
    int G() { return g; };
    int B() { return b; };

    Point3f &GetBase() { return base; };
    Point3f &GetTarget() { return target; };
    Vector3f &GetDirection() { return direction; };

    virtual Vector3f &CalcLightRay(const Point3f &p) = 0;
};
