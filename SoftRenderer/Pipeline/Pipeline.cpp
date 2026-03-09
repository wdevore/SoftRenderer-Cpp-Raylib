#include <iostream>
#include <cmath>

#include "Pipeline.h"

Pipeline::~Pipeline()
{
    std::cout << "Pipeline destroyed" << std::endl;
}

void Pipeline::Initialize(std::unique_ptr<Database> db)
{
    this->db = std::move(db);

    clipRectangle = Maths::Rectangle(0, 0, width - 1, height - 1);
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

    std::cout << "Pipeline initialized" << std::endl;
}

void Pipeline::Setup()
{
    canvas.initialize(width, height);
    painting.Initialize(width, height);

    camera.Resize(width, height);
    canvas.SetClearColor(WHITE);

    // Set Perspective
    float near = 1.0f;
    float far = 100.0f;

    float aspectRatio = CalcAspectRatio();
    // First set the perspective so we can retrieve the projection matrix.
    frustum.SetPerspective(45.0f, aspectRatio, near, far);

    // Setup the view-volume matrix (aka projection matrix)
    viewToVolume.set(frustum.projection);

    // Create and initialize camera
    camera.LookAt(0.0f, 0.0f, 15.0f, 0.0f, 0.0f, 0.0f);

    SetViewSpaceMatrix(camera.GetTransformMatrix4f());

    worldPlaneX.SetNormal(-1.0f, 0.0f, 0.0f);
    Object3D *o = db->GetObject("Plane");
    if (o != nullptr)
    {
        worldPlaneX.SetPoint(o->position);
    }

    std::cout << "Pipeline setup complete." << std::endl;
}

void Pipeline::Begin()
{
    canvas.Clear();
}

void Pipeline::Update()
{
    camera.Update();
    SetViewSpaceMatrix(camera.GetTransformMatrix4f()); // TODO may remove this in favor of the methods
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

        switch (obj->GetType())
        {
        case Object3D::ObjectType::Line:
        {
            auto lo = static_cast<LineObject *>(obj.get());
            RenderLineObject(lo);
            break;
        }
        case Object3D::ObjectType::WireMesh:
        {
            auto mo = static_cast<WireMeshObject *>(obj.get());
            RenderWireMeshObject(mo);
            break;
        }

        default:
            break;
        }
    }
}

void Pipeline::RenderLineObject(LineObject *lo)
{
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
    // we perform this: worldToView*modelToWorl
    transform.setIdentity();
    transform.set(worldToView);
    transform.mul(modelToWorld);

    RenderLine(lo->vertices[0], lo->vertices[1], lo->colorType, lo->color, lo->wuColor);
}

void Pipeline::RenderLine(const Vertex3f &vP, const Vertex3f &vQ, Object3D::ColorType colorType, PaintColoring::CColor &color, PaintColoring::WuColor &wuColor)
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
        // clipped line. But only if they were clipped
        zrP = 1.0f / p0.z;
        zrQ = 1.0f / p1.z;
        dzr = zrQ - zrP;
        dx = (p1.x - p0.x);
        // Note: I could also use Y instead
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

    // Draw the potentially clipped line.

    if (colorType == Object3D::ColorType::Color)
    {
        painting.DrawZBresenhamLine(canvas, std::round(clP.x), std::round(clP.y), std::round(clQ.x), std::round(clQ.y), p0.z, p1.z, color);
    }
    else
    {
        painting.DrawZWuBlendedLine(canvas, std::round(clP.x), std::round(clP.y), std::round(clQ.x), std::round(clQ.y), p0.z, p1.z, wuColor);
    }
}

void Pipeline::RenderWireMeshObject(WireMeshObject *mo)
{
    // Ask the object for a matrix that will transform it
    // from [object/model/local]-space to world-space.
    modelToWorld = mo->GetModelToWorldMatrix();

    // Transform the vertices from model-space to world-space and
    // then into view/camera-space.
    //
    // Note: the order of the multiplication using column-major
    // matrices; post multiplying.
    //
    // Instead of performing this: modelToWorld*worldToView
    // we perform this: worldToView*modelToWorld
    transform.setIdentity();
    transform.set(worldToView);
    transform.mul(modelToWorld);
    // std::cout << "transform: " << transform << std::endl;

    RenderAsWireFrame(mo);
}

void Pipeline::RenderAsWireFrame(WireMeshObject *mo)
{
    auto &v = mo->vertices;
    PaintColoring::CColor normalColor = PaintColoring::CColor::Red;

    // Calculate Camera Position in Model Space
    // transform contains ModelView matrix at this point
    modelViewInv.set(transform);
    modelViewInv.invert();
    modelViewInv.transform(origin, cameraModel);

    for (auto &t : mo->triangles)
    {
        // 1. Backface Culling (Model Space)
        auto n = t->GetNormal();
        v2.set(cameraModel);
        v2.sub(t->GetCenter());

        float dot = n.dot(v2);

        if (dot < 0.0f)
        {
            continue;
        }

        // 2. Render Triangle Edges
        RenderLine(v[t->i1], v[t->i2], mo->colorType, mo->color, mo->wuColor);
        RenderLine(v[t->i2], v[t->i3], mo->colorType, mo->color, mo->wuColor);
        RenderLine(v[t->i3], v[t->i1], mo->colorType, mo->color, mo->wuColor);

        // 3. Render Normals
        // Construct the normal line segment in Model Space
        Vertex3f normalStart = t->GetCenter();

        // Scale normal
        scaledNormal.set(n);
        scaledNormal.scale(0.2f); // Length of normal visual

        normalEnd.set(normalStart);
        normalEnd.add(scaledNormal);

        RenderLine(normalStart, normalEnd, Object3D::ColorType::Color, normalColor, wuNormalColor);
    }
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
///        1 = one point clipped
//         2 = none were clipped
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

// ======================== View ========================================

/// @brief Tranform from view-volume to screen-space using an affine transformation.
/// @param v
/// @param o output
void Pipeline::ViewportTransform(const Point3f &v, Point3f &o)
{
    float x;
    float y;
    if (v.z == 0.0f)
    {
        x = v.x;
        y = v.y;
    }
    else
    {
        // Perspective divide
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

// ======================== Camera ========================================
void Pipeline::MoveCameraBase(float dx, float dy, float dz)
{
    camera.MoveCameraBase(dx, dy, dz);
    SetViewSpaceMatrix(camera.GetTransformMatrix4f());
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
