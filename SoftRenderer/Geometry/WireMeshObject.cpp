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

/// @brief
/// @param i1 is 1-based
/// @param i2 is 1-based
/// @param i3 is 1-based
void WireMeshObject::AddTriangle(int i1, int i2, int i3)
{
    std::unique_ptr<Triangle> t(std::make_unique<Triangle>(i1 - 1, i2 - 1, i3 - 1));
    t->Initialize();

    t->SetCenter(
        vertices[i1 - 1],
        vertices[i2 - 1],
        vertices[i3 - 1]);
    t->SetNormal(
        vertices[i1 - 1],
        vertices[i2 - 1],
        vertices[i3 - 1]);
    std::cout << "Add Triangle: " << *t << std::endl;

    triangles.push_back(std::move(t));
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
