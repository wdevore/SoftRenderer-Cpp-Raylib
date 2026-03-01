#include "WireMeshObject.h"

WireMeshObject::WireMeshObject(/* args */)
{
}

WireMeshObject::WireMeshObject(std::string name)
{
    this->name = name;
}

WireMeshObject::~WireMeshObject()
{
    name = "MeshObject";
}

void WireMeshObject::Initialize(CColor bg, CColor fg, int intensityBits)
{
    wuC.initialize(bg, fg, intensityBits);
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
    std::unique_ptr<Triangle> t(new Triangle(i1 - 1, i2 - 1, i3 - 1));
    t->Initialize();

    t->SetCenter(
        (Vertex3f)vertices[i1 - 1],
        (Vertex3f)vertices[i2 - 1],
        (Vertex3f)vertices[i3 - 1]);
    t->SetNormal(
        (Vertex3f)vertices[i1 - 1],
        (Vertex3f)vertices[i2 - 1],
        (Vertex3f)vertices[i3 - 1]);

    triangles.push_back(std::move(t));
}

int WireMeshObject::AddEdge(EdgeProperty permanent, EdgeProperty visibility)
{
    std::unique_ptr<Edge> e(new Edge(visibility, permanent));

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
