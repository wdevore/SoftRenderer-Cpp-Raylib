#pragma once
#include <memory>

#include "Canvas.h"
#include "Painter.h"

class Pipeline
{
private:
    int width{};
    int height{};

    /// @brief Pixel buffer
    Canvas canvas{};
    Painter painter{};

public:
    // std::vector<Vector3f> vertices{};

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

    void SimpleBresenhamLine(int x0, int y0, int x1, int y1, Color color);

    // =========== Camera manipulation =================
    void MoveCameraBase(float dx, float dy, float dz);

    void OnMouseDown(int x, int y);
    void OnMouseUp();
    void OnMouseMove(int x, int y);
};
