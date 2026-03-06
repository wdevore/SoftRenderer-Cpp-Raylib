#include <iostream>
#include <cmath>

#include "Pipeline.h"
#include "LineObject.h"
#include "Painting.h"

Pipeline::~Pipeline()
{
    std::cout << "Pipeline destroyed." << std::endl;
}

void Pipeline::Initialize(std::unique_ptr<Database> db)
{
    this->db = std::move(db);

    clipRectangle = Maths::Rectangle(0, 0, width - 1, height - 1);

    // Create and initialize camera
    // camera.lookAt(0.0f, 0.0f, 7.0f, 0.0f, 0.0f, 0.0f);

    // SetViewSpaceMatrix(camera.getTransformMatrix4f());
}

void Pipeline::InitComplete()
{
    std::cout << "Initializing pipeline..." << std::endl;

    int vertexCount = db->GetVertexCount();
    int triCount = db->GetTriCount();

    std::cout << "Vertex count: " << vertexCount << std::endl;
    std::cout << "Triangle count: " << triCount << std::endl;
    std::cout << "Total edges: " << (vertexCount + triCount * 3) << std::endl;

    transformedVertices.resize(vertexCount);

    // Setup the view-volume matrix (aka projection matrix)
    viewToVolume.set(frustum.projection);

    std::cout << "Pipeline initialized." << std::endl;
}

void Pipeline::Setup()
{
    canvas.initialize(width, height);
    painting.Initialize(width, height);

    canvas.SetClearColor(WHITE);
}

void Pipeline::Begin()
{
    canvas.Clear();
}

void Pipeline::Update()
{
    canvas.Update();
    painting.Update();
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

void Pipeline::Render()
{
    for (auto &obj : db->GetObjects())
    {
        // Reset the object from previous rendering pass.
        obj->reset();

        if (obj->IsOfType(Object3D::ObjectType::Line))
        {
            auto lo = static_cast<LineObject *>(obj.get());
            RenderLineObject(lo);
        }
    }
}

void Pipeline::RenderLineObject(LineObject *lo)
{
    // painting.DrawBresenhamLine(canvas, lo->vertices[0].x, lo->vertices[0].y, lo->vertices[1].x, lo->vertices[1].y, lo->color);
    // return;

    // Ask the object for a matrix that will transform it
    // from [object/model/local]-space to world-space.
    modelToWorld = lo->GetModelToWorldMatrix();

    // Transform the vertices from model-space to world-space and
    // then into view/camera-space.
    //
    // Note: the order of the multiplication using column-major
    // matrices; post multiplying.
    //
    // Instead of performing this: modelToWorld*worldToView
    // we perform this: worldToView*modelToWorld.
    transform.setIdentity();
    transform.set(worldToView);
    transform.mul(modelToWorld);

    RenderLine(lo->vertices[0], lo->vertices[1], lo->color);
}

void Pipeline::RenderLine(Vertex3f &vP, Vertex3f &vQ, PaintColoring::CColor &color)
{
    transform.transform(vP, vOut1); // affine point transform
    transform.transform(vQ, vOut2);

    // Now 3D clip the transformed vertices to the view-volume.
    // The view-volume is currently in camera-space.
    int clipStatus = frustum.nearPlane.ClipToFront(vOut1, vOut2, p2, p3);
    if (clipStatus < 2)
    {
        // The line is not visible, nothing to render.
        return;
    }

    // Now complete the tranform on the clipped 3D fragments by
    // transforming into view-volume-space using the frustum's
    // matrix.
    viewToVolume.transform(p2, vOut1);
    vOut1.transformed = true;

    viewToVolume.transform(p3, vOut2);
    vOut2.transformed = true;

    // Now transform from view-volume-space to viewport-space.
    ViewportTransform(vOut1, p0);
    ViewportTransform(vOut2, p1);

    // Clip 2D line to viewport
    int viewportClipStatus = ClipLine(p0.x, p0.y, p1.x, p1.y, clP, clQ);

    switch (viewportClipStatus)
    {
    case 0:
        // line is not visible or both end points where off screen.
        return;
    case 1:
        // Now we need to calculate the new Z components of the
        // clipped line. But only if they were clipped.
        zrP = 1.0f / p0.z;
        zrQ = 1.0f / p1.z;
        dzr = zrQ - zrP;
        dx = (p1.x - p0.x);
        // Note: I could also use Y instead.
        // float dy = (p1.y - p0.y);
        // Just make sure you use the corresponding "y" components.
        // first point where u = 0
        p0.z = zrP + ((float)(clP.x - p0.x) / dx) * (dzr);
        p0.z = 1.0f / p0.z;
        // second point where u = 1
        p1.z = zrQ + ((float)(clQ.x - p1.x) / dx) * (dzr);
        p1.z = 1.0f / p1.z;
        break;
    case 2:
        // Z components are unchanged nothing to do.
        break;
    }

    // Draw the potentially clipped line
    painting.DrawZBresenhamLine(canvas, std::round(clP.x), std::round(clP.y), std::round(clQ.x), std::round(clQ.y), p0.z, p1.z, color);
    // drawZBresenhamLine(Math.round(clP.x), Math.round(clP.y), Math.round(clQ.x), Math.round(clQ.y), p0.z, p1.z, wc.color);
}

/// @brief Tranform from view-volume to screen-space using an affine transformation.
/// @param v
/// @param o output
void Pipeline::ViewportTransform(const Point3f &v, Point3f &o)
{
    // Perspective divide
    float x;
    float y;
    if (v.z == 0.0f)
    {
        x = v.x;
        y = v.y;
    }
    else
    {
        x = v.x / v.z;
        y = v.y / v.z;
    }

    // Viewport scale and map
    x = (width / 2.0f) * (x + 1.0f);
    y = (height / 2.0f) * (y + 1.0f);

    // Device specific vertical flip
    y = height - y;
    o.set(x, y, v.z);
}

// ======================== Clipping ========================================
/// @brief Generates a 4 bit code. Each bit indicates if x and/or y outside
///        the clip rectangle.
/// @param x
/// @param y
/// @return
int Pipeline::CalcClipCode(float x, float y)
{
    return (x < clipRectangle.x ? 8 : 0) | (x > clipRectangle.width ? 4 : 0) | (y < clipRectangle.y ? 2 : 0) | (y > clipRectangle.height ? 1 : 0);
}

/// @brief
///        0 = both points outside of region.
///        1 = one point clipped.
//         2 = none were clipped.
/// @param Px
/// @param Py
/// @param Qx
/// @param Qy
/// @param clP
/// @param clQ
/// @return
int Pipeline::ClipLine(float Px, float Py, float Qx, float Qy, Point3f &clP, Point3f &clQ)
{
    cliP = CalcClipCode(Px, Py);
    cliQ = CalcClipCode(Qx, Qy);
    pPx = Px, pPy = Py, pQx = Qx, pQy = Qy;

    while ((cliP | cliQ) != 0)
    {
        if ((cliP & cliQ) != 0)
            return 0; // both points outside of region
        dx = Qx - Px;
        dy = Qy - Py;
        if (cliP != 0)
        {
            if ((cliP & 8) == 8)
            {
                Py += (clipRectangle.x - Px) * dy / dx;
                Px = clipRectangle.x;
            }
            else if ((cliP & 4) == 4)
            {
                Py += (clipRectangle.width - Px) * dy / dx;
                Px = clipRectangle.width;
            }
            else if ((cliP & 2) == 2)
            {
                Px += (clipRectangle.y - Py) * dx / dy;
                Py = clipRectangle.y;
            }
            else if ((cliP & 1) == 1)
            {
                Px += (clipRectangle.height - Py) * dx / dy;
                Py = clipRectangle.height;
            }
            cliP = CalcClipCode(Px, Py);
        }
        else if (cliQ != 0)
        {
            if ((cliQ & 8) == 8)
            {
                Qy += (clipRectangle.x - Qx) * dy / dx;
                Qx = clipRectangle.x;
            }
            else if ((cliQ & 4) == 4)
            {
                Qy += (clipRectangle.width - Qx) * dy / dx;
                Qx = clipRectangle.width;
            }
            else if ((cliQ & 2) == 2)
            {
                Qx += (clipRectangle.y - Qy) * dx / dy;
                Qy = clipRectangle.y;
            }
            else if ((cliQ & 1) == 1)
            {
                Qx += (clipRectangle.height - Qy) * dx / dy;
                Qy = clipRectangle.height;
            }
            cliQ = CalcClipCode(Qx, Qy);
        }
    }

    clP.set(Px, Py, 0);
    clQ.set(Qx, Qy, 0);

    if (pPx == Px && pPy == Py && pQx == Qx && pQy == Qy)
        return 2;
    else
        return 1;
}