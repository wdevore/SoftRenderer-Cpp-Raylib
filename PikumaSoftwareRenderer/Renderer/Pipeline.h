#pragma once
#include <memory>

#include "Canvas.h"
#include "Painter.h"
#include "Mesh.h"
#include "Matrix4.h"
#include "Camera.h"
#include "Polygon.h"
#include "Frustum.h"
#include "DirectionalLight.h"
#include "LineCollection.h"

class Pipeline
{
public:
    enum RenderMethod
    {
        WIRE,
        WIRE_VERTEX,
        FILL_TRIANGLE,
        FILL_TRIANGLE_WIRE,
        TEXTURED,
        TEXTURED_WIRE
    };

private:
    int width{};
    int height{};

    /// @brief Pixel buffer
    Canvas canvas{};
    Painter painter{};

    int trianglesToRenderCount{};
    std::vector<Geometry::Triangle> trianglesToRender;
    std::vector<Geometry::Triangle> trianglesAfterClipping;

    int linesToRenderCount{};
    std::vector<Geometry::Line> linesToRender;

    Matrix4 scaleMatrix{};
    Matrix4 rotationMatrixX{};
    Matrix4 rotationMatrixY{};
    Matrix4 rotationMatrixZ{};
    Matrix4 translationMatrix{};

    View::Camera camera{};
    Geometry::Frustum frustum{};

    Matrix4 projMatrix{};

    Lights::DirectionalLight light{};

    RenderMethod renderMethod = RenderMethod::WIRE;

    float deltaTime{};

public:
    std::vector<Geometry::Mesh> meshes{};
    std::vector<Geometry::LineCollection> lineCollections{};

    bool shouldCullBackfaces{true};

    Pipeline(int width, int height);
    ~Pipeline();

    void Setup();
    void Begin(float deltaTime);
    void Update();
    void End();

    int addMesh(std::unique_ptr<Geometry::Mesh> mesh);
    int addLineCollection(std::unique_ptr<Geometry::LineCollection> collection);

    void Render();
    void setRenderMethod(RenderMethod method)
    {
        renderMethod = method;
    }

    void ProcessPipelineMesh(Geometry::Mesh &mesh);
    void ProcessPipelineLines(Geometry::LineCollection &lines);

    void SimpleBresenhamLine(int x0, int y0, int x1, int y1, Color color);

    // =========== Render control =================
    bool shouldRenderWire()
    {
        return (
            renderMethod == WIRE ||
            renderMethod == WIRE_VERTEX ||
            renderMethod == FILL_TRIANGLE_WIRE ||
            renderMethod == TEXTURED_WIRE);
    }
    bool shouldRenderWireVertex()
    {
        return (renderMethod == WIRE_VERTEX);
    }
    bool shouldRenderFilledTriangle()
    {
        return (
            renderMethod == FILL_TRIANGLE ||
            renderMethod == FILL_TRIANGLE_WIRE);
    }
    bool shouldRenderTexturedTriangle()
    {
        return (
            renderMethod == TEXTURED ||
            renderMethod == TEXTURED_WIRE);
    }

    // =========== Object manipulation =================
    void rotateOnX(int index, float angle);
    void rotateOnY(int index, float angle);
    void rotateOnZ(int index, float angle);

    void setScale(int index, Maths::Vector3f scale);
    void setRotation(int index, Maths::Vector3f rotation);
    void setTranslation(int index, Maths::Vector3f translation);

    // =========== Clipping =================
    void clipPolygonAgainstPlane(Geometry::Polygon &polygon, int plane);

    // =========== Camera manipulation =================
    void MoveCameraBase(float dx, float dy, float dz);

    void OnMouseDown(int x, int y);
    void OnMouseUp();
    void OnMouseMove(int x, int y, int dx, int dy);
    void OnMouseWheel(float delta);
};
