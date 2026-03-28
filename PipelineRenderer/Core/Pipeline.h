#pragma once
#include <memory>

#include "Canvas.h"
#include "Frustum.h"
#include "ArcBall.h"
#include "ZBuffer.h"
#include "Object3D.h"
#include "Painter.h"

class Pipeline
{
private:
    int width{};
    int height{};

    /// @brief Pixel buffer
    Canvas canvas{};
    /// @brief Z buffer
    ZBuffer zb{};
    Painter painter{};

    // =========== Space mapping matricies MVP ================
    Matrix4f mvp{};

    // ----------- Project matrix (Camera) --------------
    Frustum frustum{}; // Contains Projection matrix (P)
    Matrix4f projectionMatrix{};

    // ----------- View matrix (NDC) --------------
    ArcBall camera{};
    Matrix4f viewMatrix{};

    // ----------- Model (Object) --------------
    Matrix4f modelMatrix; // Model

    std::vector<std::unique_ptr<Object3D>> objects{};

    // =========== Working vars ================
    Vector3f vOut1{};
    Vector3f vOut2{};
    Vector3f vOut3{};

public:
    std::vector<Vector3f> vertices{};

    Pipeline(int width, int height) : width(width), height(height)
    {
        painter.Initialize(width, height);
    };
    ~Pipeline();

    void Setup();
    void Begin();
    void Update();
    void End();

    void Render();
    void ProjectVertex(const Vector3f &v, Vector3f &out);

    float CalcAspectRatio();
    /// @brief Set World to View matrix (aka Model transform)
    /// @param m
    void SetViewSpaceMatrix(const Matrix4f &m);

    void AddObject(std::unique_ptr<Object3D> object);

    void SimpleBresenhamLine(int x0, int y0, int x1, int y1, Color color);

    // =========== Camera manipulation =================
    void MoveCameraBase(float dx, float dy, float dz);

    void OnMouseDown(int x, int y);
    void OnMouseUp();
    void OnMouseMove(int x, int y);
};
