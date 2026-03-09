#pragma once

#include <memory>

#include "Canvas.h"
#include "Painting.h"
#include "Frustum.h"
#include "Database.h"
#include "LineObject.h"
#include "Rectangle.h"
#include "CColor.h"
#include "WuColor.h"
#include "ArcBall.h"
#include "WireMeshObject.h"

// Pipeline takes a vector of vertices, edges and normals.
//
// The pipeline performs several sequences before final rasterization in pixel
// space. First it transforms all vertices to view-space, then
// First is transforms all vertices through different spaces until it reaches
// pixel space.

class Pipeline
{
private:
    int width{};
    int height{};

    Canvas canvas{};
    Painting painting{};

    // Frustum representing viewing volume
    Frustum frustum{};
    std::unique_ptr<Database> db;

    std::vector<Vertex3f> transformedVertices{};

    // =========== Space mapping matricies ================
    // This matrix represents the transform from view-space to view-volume-space.
    // This is usually the frustum.
    Matrix4f viewToVolume{};

    // This matrix represents the transform from model - space to world - space.
    // The *modelview matrix is stored in each object as each object potentially
    // is *located somewhere in world - space.
    Matrix4f modelToWorld;

    // This matrix represents transforms from world-space to view-space. It is
    // set by getting the camera's transform matrix.
    // camera.GetTransformMatrix();
    //
    // Note: we are talking about camera or view-space NOT view-volume-space.
    Matrix4f worldToView;

    Matrix4f modelViewInv{};
    Point3f cameraModel{};
    Point3f origin{};
    Vertex3f normalEnd{};
    Vector3f scaledNormal{};

    Matrix4f transform{};

    // Working variables
    Vertex3f vOut1{};
    Vertex3f vOut2{};
    Vector3f v1{};
    Vector3f v2{};
    Vector3f v3{};

    Point3f p0{};
    Point3f p1{};
    Point3f p2{};
    Point3f p3{};
    Point3f clP{};
    Point3f clQ{};

    float zrP{};
    float zrQ{};
    float dzr{};
    float dx{};
    float dy{};

    // Clipping variables
    int cliP{};
    int cliQ{};
    float pPx{};
    float pPy{};
    float pQx{};
    float pQy{};
    Maths::Rectangle clipRectangle{};

    // World Clipping planes
    Plane worldPlaneX{};

    ArcBall camera{};

    // ============= Debug or Visuals ===============
    PaintColoring::WuColor wuNormalColor = PaintColoring::WuColor::Red;

public:
    Pipeline(int width, int height) : width(width), height(height) {};
    ~Pipeline();

    void Initialize(std::unique_ptr<Database> db);
    void InitComplete();

    void Setup();
    void Begin();
    void Update();
    void End();

    // =========== Renderers =================
    void Render();
    void RenderLineObject(LineObject *lo);
    void RenderLine(const Vertex3f &vP, const Vertex3f &vQ, Object3D::ColorType colorType, PaintColoring::CColor &color, PaintColoring::WuColor &wuColor);
    void RenderWireMeshObject(WireMeshObject *mo);
    void RenderAsWireFrame(WireMeshObject *mo);

    // =========== Clip methods =================
    int CalcClipCode(float x, float y);
    int ClipLine(float Px, float Py, float Qx, float Qy, Point3f &clP, Point3f &clQ);

    /// @brief Set the world-space to view-space matrix. This matrix is retrieved from
    ///        the camera's transform matrix.
    /// @param m
    void SetViewSpaceMatrix(const Matrix4f &m);

    void ViewportTransform(const Point3f &v, Point3f &o);

    float CalcAspectRatio();

    // =========== Camera manipulation =================
    void MoveCameraBase(float dx, float dy, float dz);

    void OnMouseDown(int x, int y);
    void OnMouseUp();
    void OnMouseMove(int x, int y);
};

/*

        // 1. Backface Culling (View Space)

        // Transform normal to View Space (Direction only)
        auto n = t->GetNormal();
        transform.transform(n, v1);
        v1.normalize();

        // Transform center to View Space (Position)
        transform.transform(t->GetCenter(), p0);

        // Vector from Point to Camera (Camera is at 0,0,0 in View Space)
        // v2 = Camera - Point = (0,0,0) - p0 = -p0
        v2.set(-p0.x, -p0.y, -p0.z);

        float dot = v1.dot(v2);

        if (dot < 0.0f)
        {
            continue;
        }

        // 2. Render Triangle Edges
        RenderLine(v[t->i1], v[t->i2], mo->colorType, mo->color, mo->wuColor);
        RenderLine(v[t->i2], v[t->i3], mo->colorType, mo->color, mo->wuColor);
        RenderLine(v[t->i3], v[t->i1], mo->colorType, mo->color, mo->wuColor);

        // 3. Render Normal
        // Construct the normal line segment in Model Space
        Vertex3f normalStart = t->GetCenter();
        Vertex3f normalEnd;

        // Scale normal
        Vector3f scaledNormal;
        scaledNormal.set(n);
        scaledNormal.scale(0.5f); // Length of normal visual

        normalEnd.set(normalStart);
        normalEnd.add(scaledNormal);

        RenderLine(normalStart, normalEnd, Object3D::ColorType::Color, normalColor, wuNormalColor);



==============================================================
std::cout << "Triangle: " << *t << std::endl;

        // The transform the normal into view-space
        // 1. Backface Culling (View Space)

        // Transform normal to View Space (Direction only)
        auto n = t->GetNormal();
        transform.transform(n, v1); // vector transform
        transform.transform(n, v1);
        v1.normalize();
        std::cout << "v1: " << v1 << std::endl;

        // =========== form two points to draw normals ===========
        transform.transform(t->GetCenter(), p0); // point transformed
        std::cout << "p0: " << p0 << std::endl;
        p3.set(n);
        p3.scale(2.0f);
        transform.transform(p3, p1); // point transformed
        // Transform center to View Space (Position)
        transform.transform(t->GetCenter(), p0);

        // p1.add(v1, p0);
        // std::cout << "p1: " << p1 << std::endl;
        // v3.sub(p1, p0);
        // v3.normalize();
        // std::cout << "v3: " << v3 << std::endl;
        viewToVolume.transform(p0, p2);
        std::cout << "p2: " << p2 << std::endl;
        viewToVolume.transform(p1, p3);
        std::cout << "p3: " << p3 << std::endl;
        ViewportTransform(p2, p0);
        std::cout << "p0: " << p0 << std::endl;
        ViewportTransform(p3, p1);
        std::cout << "p1: " << p1 << std::endl;
        // Vector from Point to Camera (Camera is at 0,0,0 in View Space)
        // v2 = Camera - Point = (0,0,0) - p0 = -p0
        v2.set(-p0.x, -p0.y, -p0.z);

        float dot = v1.dot(v2);

        // We only clip in view-port (aka pixel-space)
        // if (ClipLine(p0.x, p0.y, p1.x, p1.y, clP, clQ))
        // {
        //     vOut1.set(std::round(clP.x), std::round(clP.y), -1.0f);
        //     vOut2.set(std::round(clQ.x), std::round(clQ.y), -1.0f);
        //     std::cout << "vOut1: " << vOut1 << std::endl;
        //     std::cout << "vOut2: " << vOut2 << std::endl;
        //     PaintColoring::CColor nc{PaintColoring::CColor::Red};
        //     RenderLine(vOut1, vOut2, mo->colorType, nc, wuNormalColor);
        // }

        // // Transform a vertex on triangle from model-space to view-space.
        // p1.set(v[t->i1]);
        // transform.transform(p1, p2); // affine point transform

        // The camera is at world origin when in view-space for OpenGL.
        // Create ray from face center towards camera position.
        v2.set(0.0f, 0.0f, frustum.GetViewDistance());
        v2.sub(p2);
        v2.normalize();

        float dot = v1.dot(v2);
        if (dot < 0.0f)
        {
            // skip to next face
            continue;
        }

        // At this point the face is visible. Now transform the triangle's
        // vertices to the viewvolume space for possibly more visiblity
        // tests. We only transform the vertex if it has not been transformed
        // during the face interation.
        // 2. Render Triangle Edges
        RenderLine(v[t->i1], v[t->i2], mo->colorType, mo->color, mo->wuColor);
        RenderLine(v[t->i2], v[t->i3], mo->colorType, mo->color, mo->wuColor);
        RenderLine(v[t->i3], v[t->i1], mo->colorType, mo->color, mo->wuColor);

        // 3. Render Normal
        // Construct the normal line segment in Model Space
        Vertex3f normalStart = t->GetCenter();
        Vertex3f normalEnd;

        // Scale normal
        Vector3f scaledNormal;
        scaledNormal.set(n);
        scaledNormal.scale(0.5f); // Length of normal visual

        normalEnd.set(normalStart);
        normalEnd.add(scaledNormal);

        RenderLine(normalStart, normalEnd, Object3D::ColorType::Color, normalColor, wuNormalColor);

*/