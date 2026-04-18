#include <iostream>
#include <cmath>

#include "Pipeline.h"

Pipeline::~Pipeline()
{
}

void Pipeline::Setup()
{
    canvas.initialize(width, height);
    // painting.Initialize(width, height);

    canvas.SetClearColor(DARKGRAY);

    std::cout << "Pipeline setup complete." << std::endl;
}

void Pipeline::Begin()
{
    canvas.Clear();
}

void Pipeline::Update()
{
    // camera.Update();
    // Because the camera is dynamic retrieve the latest Projection matrix.
    // viewMatrix.set(camera.GetTransformMatrix4f());
    canvas.Update(); // Updates color buffer

    // zb.reset();
}

void Pipeline::End()
{
    //------------------------------------------------------------------
    // Draw the Texture2D (which resides in GPU memory and was updated
    // with your pixel buffer) onto the application window
    // at coordinates (0, 0) Your PutPixel calls only change data in a
    // std::vector (RAM). canvas.Update() uploads that RAM to the GPU.
    // This line finally makes that GPU texture visible on the screen.
    //------------------------------------------------------------------
    canvas.Blit(0, 0); // Copies color buffer to screen
}

void Pipeline::Render()
{
    painter.DrawGrid(canvas, CColor::Black);
    painter.DrawRectangle(canvas, 50, 50, 200, 200, CColor::Magenta);
}

// ======================== Camera ========================================
void Pipeline::MoveCameraBase(float dx, float dy, float dz)
{
    // camera.MoveCameraBase(dx, dy, dz);
}

void Pipeline::OnMouseDown(int x, int y)
{
    // camera.OnMouseDown(x, y);
}

void Pipeline::OnMouseUp()
{
    // camera.OnMouseUp();
}

void Pipeline::OnMouseMove(int x, int y)
{
    // camera.OnMouseMove(x, y);
}
