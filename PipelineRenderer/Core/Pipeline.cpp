#include <iostream>
#include <cmath>

#include "Pipeline.h"
#include "LineObject.h"

Pipeline::~Pipeline()
{
}

void Pipeline::Setup()
{
    canvas.initialize(width, height);
    // painting.Initialize(width, height);

    canvas.SetClearColor(WHITE);

    // Set Perspective
    float near = -1.0f;
    float far = -2.5f;
    camera.Resize(width, height);

    // =============================================
    // Model/View/Projection Matrix setup
    // Model: triangle's transform      <== Changes on transformations
    // View: camera's transform         <== Changes on transformations
    // Projection: frustum's transform  <== Constant
    // =============================================

    // --------- View ------------------------------
    // The View matrix is based on the camera. Create and initialize camera
    // (aka ArcBall) looking down the -Z axis
    camera.LookAt(0.0f, 0.0f, 5.0f, 0.0f, 0.0f, 0.0f);

    // --------- Projection ------------------------
    float aspectRatio = CalcAspectRatio();
    // First set the perspective so we can retrieve the projection matrix.
    frustum.SetPerspective(45.0f, aspectRatio, near, far);

    // Setup the view-volume matrix (aka projection matrix)
    projectionMatrix.set(frustum.projection);

    // --------- Model ------------------------
    modelMatrix.setIdentity();

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
    // Because the camera is dynamic retrieve the latest Projection matrix.
    viewMatrix.set(camera.GetTransformMatrix4f());
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
    for (auto &obj : objects)
    {
        // For each object retrieve the Model matrix.
        switch (obj->GetType())
        {
        case Object3D::ObjectType::Line:
        {
            // auto o = static_cast<LineObject *>(obj.get());
            // Build mvp. Get Object's Model matrix (identity for now)
            modelMatrix = obj->GetModel();
            mvp.mul(modelMatrix, viewMatrix);
            mvp.mul(projectionMatrix);
            // mvp.mul(modelMatrix, projectionMatrix);
            // mvp.mul(viewMatrix);

            // Map to screen space
            ProjectVertex(vertices[0], vOut1);
            ProjectVertex(vertices[1], vOut2);

            // Draw
            painter.DrawZLine(canvas, zb, vOut1, vOut2, CColor::Blue);
            break;
        }
        case Object3D::ObjectType::WireMesh:
        {
            modelMatrix = obj->GetModel();
            mvp.mul(modelMatrix, viewMatrix);
            mvp.mul(projectionMatrix);

            for (auto &f : obj->faces)
            {
                // Map to screen space
                ProjectVertex(vertices[f.i1], vOut1);
                ProjectVertex(vertices[f.i2], vOut2);
                ProjectVertex(vertices[f.i3], vOut3);

                // Draw
                painter.DrawZLine(canvas, zb, vOut1, vOut2, CColor::Blue);
                painter.DrawZLine(canvas, zb, vOut2, vOut3, CColor::Red);
                painter.DrawZLine(canvas, zb, vOut3, vOut1, CColor::Green);
            }
            break;
        }
        case Object3D::ObjectType::FlatShaded:
        {
            break;
        }

        default:
            break;
        }
    }
}

void Pipeline::ProjectVertex(const Vector3f &v, Vector3f &out)
{
    // 1. Transform vertex by Model-View-Projection matrix

    mvp.transform(v, out);

    // float x = v.x * mvp.m00 + v.y * mvp.m01 + v.z * mvp.m02 + mvp.m03;
    // float y = v.x * mvp.m10 + v.y * mvp.m11 + v.z * mvp.m12 + mvp.m13;
    // float z = v.x * mvp.m20 + v.y * mvp.m21 + v.z * mvp.m22 + mvp.m23;
    // float w = v.x * mvp.m30 + v.y * mvp.m31 + v.z * mvp.m32 + mvp.m33;

    // // 2. Perspective Divide (Convert to Normalized Device Coordinates -1 to 1)
    // if (w != 0.0f)
    // {
    //     x /= w;
    //     y /= w;
    //     z /= w;
    // }

    // 3. Viewport Transform (Convert NDC to Screen Pixels)
    out.set(
        (out.x + 1.0f) * 0.5f * width,
        (1.0f - out.y) * 0.5f * height, // Flip Y because screen Y grows downward
        out.z                           // Keep Z for the Z-buffer
    );

    // out.set(
    //     (x + 1.0f) * 0.5f * width,
    //     (1.0f - y) * 0.5f * height, // Flip Y because screen Y grows downward
    //     z                           // Keep Z for the Z-buffer
    // );
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

// TODO This isn't correct
void Pipeline::SetViewSpaceMatrix(const Matrix4f &m)
{
    // modelMatrix.set(m);
}

void Pipeline::AddObject(std::unique_ptr<Object3D> object)
{
    if (object)
    {
        objects.push_back(std::move(object));
    }
}

void Pipeline::SimpleBresenhamLine(int x0, int y0, int x1, int y1, Color color)
{
    int dx = abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
    int dy = -abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
    int err = dx + dy, e2;

    while (true)
    {
        // Pixel plotting with bounds check
        if (x0 >= 0 && x0 < width && y0 >= 0 && y0 < height)
        {
            canvas.PutPixel(x0, y0, color);
        }

        if (x0 == x1 && y0 == y1)
            break;
        e2 = 2 * err;
        if (e2 >= dy)
        {
            err += dy;
            x0 += sx;
        }
        if (e2 <= dx)
        {
            err += dx;
            y0 += sy;
        }
    }
}

// ======================== Camera ========================================
void Pipeline::MoveCameraBase(float dx, float dy, float dz)
{
    camera.MoveCameraBase(dx, dy, dz);
    // SetViewSpaceMatrix(camera.GetTransformMatrix4f());
    // viewMatrix.set(camera.GetTransformMatrix4f());
}

void Pipeline::OnMouseDown(int x, int y)
{
    camera.OnMouseDown(x, y);
}

void Pipeline::OnMouseUp()
{
    camera.OnMouseUp();
}

void Pipeline::OnMouseMove(int x, int y)
{
    camera.OnMouseMove(x, y);
}
