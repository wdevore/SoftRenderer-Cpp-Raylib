#pragma once

#include "Canvas.h"
#include "Frustum.h"
#include "ArcBall.h"
#include "ZBuffer.h"

class Pipeline
{
private:
    /* data */
    int width{};
    int height{};

    Canvas canvas{};

    // =========== Space mapping matricies MVP ================
    Matrix4f mvp{};

    // Project matrix
    Frustum frustum{}; // Contains Projection matrix (P)

    // This matrix represents the transform from view-space to view-volume-space.
    // This is usually the frustum. (V)
    Matrix4f viewToVolume{};

    // This matrix represents transforms from world-space to view-space. It is
    // set by getting the camera's transform matrix. (M)
    // camera.GetTransformMatrix();
    //
    // Note: we are talking about camera or view-space NOT view-volume-space.
    Matrix4f worldToView;

    ArcBall camera{};

    // Z buffer vars
    ZBuffer zb{};

public:
    Pipeline(int width, int height) : width(width), height(height) {};
    ~Pipeline();

    void Setup();
    void Begin();
    void Update();
    void End();

    void Render();

    float CalcAspectRatio();
    /// @brief Set World to View matrix (aka Model transform)
    /// @param m
    void SetViewSpaceMatrix(const Matrix4f &m);
};
