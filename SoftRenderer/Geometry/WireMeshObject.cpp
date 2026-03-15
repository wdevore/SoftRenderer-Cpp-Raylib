#include <iostream>

#include "WireMeshObject.h"

WireMeshObject::WireMeshObject(/* args */)
{
    type = ObjectType::WireMesh;
}

WireMeshObject::WireMeshObject(std::string name)
{
    type = ObjectType::WireMesh;
    this->name = name;
}

WireMeshObject::~WireMeshObject()
{
    name = "MeshObject";
}

void WireMeshObject::Initialize(PaintColoring::CColor bg, PaintColoring::CColor fg, int intensityBits)
{
    color = fg;
    wuColor.initialize(bg, fg, intensityBits);
}

void WireMeshObject::SetAnimate(bool animate)
{
    this->animate = animate;
}

void WireMeshObject::AddVertex(Vertex3f v)
{
    vertices.push_back(v);
}

Triangle &WireMeshObject::GetTriangle(int index)
{
    if (index < 0)
    {
        index = triangles.size() - 1;
    }
    return *triangles[index];
}

int WireMeshObject::AddEdge(EdgeProperty permanent, EdgeProperty visibility)
{
    std::unique_ptr<Edge> e(std::make_unique<Edge>(visibility, permanent));

    edges.push_back(std::move(e));

    return edges.size();
}

void WireMeshObject::ResetEdgeList()
{
    for (auto &e : edges)
    {
        e->face = NOFACE;
    }
}
