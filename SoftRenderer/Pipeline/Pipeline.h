#pragma once

#include <memory>

#include "Canvas.h"
#include "Painting.h"
#include "Frustum.h"
#include "Database.h"
#include "LineObject.h"
#include "Rectangle.h"
#include "CColor.h"

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

    // ArcBall camera;

    // This matrix represents the transform from view-space to view-volume-space.
    // This is usually the frustum.
    Matrix4f viewToVolume{};

    // Space mapping matricies
    Matrix4f modelToWorld;
    Matrix4f worldToView;

    Matrix4f transform;

    // Working variables
    Vertex3f vOut1{};
    Vertex3f vOut2{};

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
    void RenderLine(Vertex3f &vP, Vertex3f &vQ, PaintColoring::CColor &color);
    void ViewportTransform(const Point3f &v, Point3f &o);

    // Clip methods
    int CalcClipCode(float x, float y);
    int ClipLine(float Px, float Py, float Qx, float Qy, Point3f &clP, Point3f &clQ);
};
