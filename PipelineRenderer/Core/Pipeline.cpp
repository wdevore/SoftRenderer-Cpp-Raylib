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

    canvas.SetClearColor(WHITE);

    // Set Perspective
    float near = 1.0f;
    float far = 100.0f;
    camera.Resize(width, height);

    // =============================================
    // Model/View/Projection Matrix setup
    // Model: triangle's transform      <== Changes on transformations
    // View: camera's transform         <== Changes on transformations
    // Projection: frustum's transform  <== Constant
    // =============================================

    // --------- View ------------------------------
    // The View matrix is based on the camera. Create and initialize camera
    // (aka ArcBall)
    camera.LookAt(0.0f, 0.0f, 15.0f, 0.0f, 0.0f, 0.0f);
    SetViewSpaceMatrix(camera.GetTransformMatrix4f());

    // --------- Projection ------------------------
    float aspectRatio = CalcAspectRatio();
    // First set the perspective so we can retrieve the projection matrix.
    frustum.SetPerspective(45.0f, aspectRatio, near, far);

    // Setup the view-volume matrix (aka projection matrix)
    viewToVolume.set(frustum.projection);

    zb.Initialize(width, height);

    std::cout << "Pipeline setup complete." << std::endl;
}

void Pipeline::Begin()
{
    canvas.Clear();
}

void Pipeline::Update()
{
    camera.Update();
    // SetViewSpaceMatrix(camera.GetTransformMatrix4f()); // TODO may remove this in favor of the methods
    canvas.Update(); // Updates color buffer

    zb.reset();
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
    // Because the camera is dynamic retrieve the latest Projection matrix.

    // Loop through all objects.
    //    For each object retrieve the Model matrix.
}

float Pipeline::CalcAspectRatio()
{
    float aspectRatio;
    if (width > height)
    {
        aspectRatio = (float)width / (float)height;
    }
    else
    {
        aspectRatio = (float)height / (float)width;
    }

    return aspectRatio;
}

void Pipeline::SetViewSpaceMatrix(const Matrix4f &m)
{
    worldToView.set(m);
}
