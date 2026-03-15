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

    // camera.Resize(width, height);
    canvas.SetClearColor(WHITE);

    // Set Perspective
    float near = 1.0f;
    float far = 100.0f;

    // float aspectRatio = CalcAspectRatio();
    // // First set the perspective so we can retrieve the projection matrix.
    // frustum.SetPerspective(45.0f, aspectRatio, near, far);

    // // Setup the view-volume matrix (aka projection matrix)
    // viewToVolume.set(frustum.projection);

    // // Create and initialize camera
    // camera.LookAt(0.0f, 0.0f, 15.0f, 0.0f, 0.0f, 0.0f);

    // SetViewSpaceMatrix(camera.GetTransformMatrix4f());

    // worldPlaneX.SetNormal(-1.0f, 0.0f, 0.0f);
    // Object3D *o = db->GetObject("Plane");
    // if (o != nullptr)
    // {
    //     worldPlaneX.SetPoint(o->position);
    // }

    std::cout << "Pipeline setup complete." << std::endl;
}

void Pipeline::Begin()
{
    canvas.Clear();
}

void Pipeline::Update()
{
    // camera.Update();
    // SetViewSpaceMatrix(camera.GetTransformMatrix4f()); // TODO may remove this in favor of the methods
    canvas.Update();
    // painting.Update();
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
    canvas.Blit(0, 0);
}