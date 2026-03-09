#pragma once

#include <string>
#include <memory>

#include "Object3D.h"
#include "Vertex3f.h"
#include "WuColor.h"
#include "Triangle.h"
#include "Edge.h"

class WireMeshObject : public Object3D
{
private:
    /* data */
public:
    PaintColoring::WuColor wuColor{};
    PaintColoring::CColor color{};

    std::vector<std::unique_ptr<Triangle>> triangles{};
    // A simple edge visibility collection
    std::vector<std::unique_ptr<Edge>> edges{};

    WireMeshObject(/* args */);
    WireMeshObject(std::string name);
    ~WireMeshObject();

    void Initialize(PaintColoring::CColor bg, PaintColoring::CColor fg, int intensityBits);
    void SetAnimate(bool animate) override;

    int GetTriangleCount() { return triangles.size(); }
    void AddVertex(Vertex3f v);
    void AddTriangle(int i1, int i2, int i3);
    /// @brief
    /// @param index if -1 then return last added Triangle
    /// @return
    Triangle &GetTriangle(int index = -1);

    int AddEdge(EdgeProperty permanent, EdgeProperty visibility);
    void ResetEdgeList();
};
