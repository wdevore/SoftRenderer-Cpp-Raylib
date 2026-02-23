#pragma once

#include <vector>

#include "Quat4f.h"
#include "Vector3f.h"
#include "Matrix4f.h"

class ArcBall
{
    const static int NoAxes = 0;
    const static int LG_NSEGS = 4;
    const static int NSEGS = 1 << LG_NSEGS;

private:
    float fRadius{};
    Quat4f qNow{};
    Quat4f qDown{};
    Quat4f qDrag{};
    Vector3f vNow{};
    Vector3f vDown{};
    Vector3f vFrom{};
    Vector3f vTo{};
    Vector3f vrFrom{};
    Vector3f vrTo{};
    Matrix4f mNow{};
    Matrix4f mDown{};

    bool bDragging{false};

    // Canvas size
    int width{};
    int height{};

    Vector3f vBallMouse{};
    Quat4f q{};
    Quat4f qConj{};
    std::vector<Vector3f> pts{};
    Vector3f vVector{};
    Vector3f vBase{};
    Vector3f vDirection{};
    Vector3f vCanvasCenter{};
    AxisAngle4f aaYaxis = AxisAngle4f(0.0f, 1.0f, 0.0f, 180.0f / 57.3f);
    AxisAngle4f aa{};

    Point3f p1{};
    Point3f p2{};

    // Ken's arcball must be based on 2D screen coords where the +y is upwards respectively.
    // However, SWT 2D screen coords are such that +y is downward.
    // In order to sync with Ken's arcball I flip the screen coords to match Ken's
    // y = (Height - y).
    // Also, a conjugate is needed as well as a (-y).
    bool bScreenYOrientation{true}; // default to GL, Y is upwards.

public:
    Vector3f vLos{};
    Vector3f vUp{};
    Vector3f vHorz{};

    // Position represented as a matrix
    Matrix4f translation{};
    // Orientation is expressed as a series of rotations
    Matrix4f rotation{};
    // The combined rotation and translation.
    Matrix4f affineTransform{};

    ArcBall(/* args */);
    ~ArcBall();

    void initialize();
    void reset();
};
