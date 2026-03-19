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
    // Canvas size
    int width{};
    int height{};

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

    Vector3f vBallMouse{};
    Quat4f vBallPoint{};
    Quat4f q{};
    Quat4f qConj{};
    std::vector<Vector3f> pts{};
    Vector3f vVector{};
    Vector3f vBase{};
    Vector3f vDirection{};
    Vector3f vCanvasCenter{};
    AxisAngle4f aaYaxis = AxisAngle4f(0.0f, 1.0f, 0.0f, 180.0f / 57.3f);
    AxisAngle4f aa{};

    Vector3f p1{};
    Vector3f p2{};
    Vector3f vWork{};

    // Ken's arcball must be based on 2D screen coords where the +y is upwards respectively.
    // However, SWT 2D screen coords are such that +y is downward.
    // In order to sync with Ken's arcball I flip the screen coords to match Ken's
    // y = (Height - y).
    // Also, a conjugate is needed as well as a (-y).
    bool bScreenYOrientation{true}; // default to GL, Y is upwards.

public:
    Vector3f vLos{};    // Forward/backward
    Vector3f vUp{};     // Up/down
    Vector3f vHorz{};   // Left/Right
    Vector3f vTarget{}; // Point of focus

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

    void Place(Vector3f v, float r);
    void Resize(int width, int height);
    void MapScreenCoords(int x, int y);

    void Update();

    /// @brief Map window coordinates to Arcball coords. Output is vBallMouse.
    /// @param vMouse
    void MapMouseToSphere(const Vector3f &vMouse);

    /// @brief Construct a unit quaternion from two points on unit sphere.
    /// @param vFrom
    /// @param vTo
    /// @return results is in "vBallPoint" property
    void MapFromBallPoints(const Vector3f &vFrom, const Vector3f &vTo);

    /// @brief Convert a unit quaternion to two points on unit sphere.
    /// @param q
    /// @param vArcFrom Modified output
    /// @param vArcTo Modified output
    void MapToBallPoints(const Quat4f &q, Vector3f &vArcFrom, Vector3f &vArcTo);

    /// @brief Convert window coordinates to sphere coordinates.
    /// @param vMouse
    /// @return results is in vBallMouse
    void MouseOnSphere(const Vector3f &vMouse);

    void SetBScreenYOrientation(bool bScreenYOrientation) { this->bScreenYOrientation = bScreenYOrientation; }

    void OnMouseDown(int x, int y);
    void OnMouseUp();
    void OnMouseMove(int x, int y);

    /// @brief
    /// @param vDir The direction to rotate to.
    void LookInDirection(const Vector3f &vDir);

    /// @brief The base is assumed to be the world origin.
    /// @param vTarget where the camera is to look at.
    void LookAt(const Vector3f &vTarget);

    /// @brief The base is assumed to be the world origin.
    /// @param vBase
    /// @param vTarget
    void LookAt(const Vector3f &vBase, const Vector3f &vTarget);

    /// @brief The base is assumed to be the world origin.
    /// @param xb
    /// @param yb
    /// @param zb
    /// @param xt
    /// @param yt
    /// @param zt
    void LookAt(float xb, float yb, float zb, float xt, float yt, float zt);

    /// @brief Calulated Line of Sight and stores in "vLos"
    void ComputeLOS();

    /// @brief Generates a Model matrix
    /// @return a Model matrix as part of the (M)VP matrix composition.
    Matrix4f &GetTransformMatrix4f();
    /// @brief This method is generally for computing visibility which means the
    ///        position returned must be in world-space.
    /// @return
    Vector3f &GetWorldPosition();
    Vector3f &GetDirection() { return vDirection; }

    void MoveCameraBase(float dx, float dy, float dz);
};
