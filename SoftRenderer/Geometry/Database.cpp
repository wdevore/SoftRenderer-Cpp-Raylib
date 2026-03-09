#include "Database.h"
#include "WireMeshObject.h"
#include "LineObject.h"

Database::Database(/* args */)
{
}

Database::~Database()
{
}

Object3D *Database::GetObject(std::string name)
{
    for (auto &o : objects)
    {
        if (o->name == name)
        {
            return o.get();
        }
    }
    return nullptr;
}

LightBase *Database::GetLight(int index)
{
    if (index >= 0 && index < lights.size())
    {
        return lights[index].get();
    }
    return nullptr;
}

void Database::AddObject(std::unique_ptr<Object3D> o)
{
    if (o)
    {
        // Track the vertex count
        vertexCount += o->GetVertexCount();

        // Track the face count
        WireMeshObject *mo = dynamic_cast<WireMeshObject *>(o.get());
        if (mo)
        {
            triCount += mo->GetTriangleCount();
        }

        // Even though o is passed by value
        // (and thus ownership is already in the function scope),
        // o itself is a named variable, which makes it an l-value.
        // To put it into the objects vector (which requires taking ownership),
        // you must cast it to an r-value using std::move.
        objects.push_back(std::move(o));
    }
}

void Database::AddLine(std::string name,
                       float px, float py, float pz,
                       float qx, float qy, float qz,
                       const PaintColoring::CColor &color)
{
    std::unique_ptr<Object3D> o;

    o = std::make_unique<LineObject>(PaintColoring::CColor::White, color);
    o->name = name;
    o->addVertex(Vertex3f{px, py, pz});
    o->addVertex(Vertex3f{qx, qy, qz});
    o->colorType = Object3D::ColorType::Color;
    AddObject(std::move(o));
}

void Database::AddTripodAxies(float scale)
{
    std::unique_ptr<Object3D> o;

    o = std::make_unique<LineObject>(PaintColoring::CColor::White, PaintColoring::CColor::Red);
    o->name = "+X";
    o->addVertex(Vertex3f{0.0f, 0.05f, 0.0f});
    o->addVertex(Vertex3f{scale, 0.05f, 0.0f});
    AddObject(std::move(o));

    o = std::make_unique<LineObject>(PaintColoring::CColor::White, PaintColoring::CColor::Yellow);
    o->name = "-X";
    o->addVertex(Vertex3f{0.0f, 0.05f, 0.0f});
    o->addVertex(Vertex3f{-scale, 0.05f, 0.0f});
    AddObject(std::move(o));

    o = std::make_unique<LineObject>(PaintColoring::CColor::White, PaintColoring::CColor::Green);
    o->name = "+Z";
    o->addVertex(Vertex3f{0.0f, 0.05f, 0.0f});
    o->addVertex(Vertex3f{0.0f, 0.05f, scale});
    AddObject(std::move(o));

    o = std::make_unique<LineObject>(PaintColoring::CColor::White, PaintColoring::CColor::Black);
    o->name = "-Z";
    o->addVertex(Vertex3f{0.0f, 0.05f, 0.0f});
    o->addVertex(Vertex3f{0.0f, 0.05f, -scale});
    AddObject(std::move(o));

    o = std::make_unique<LineObject>(PaintColoring::CColor::White, PaintColoring::CColor::Blue);
    o->name = "+Y";
    o->addVertex(Vertex3f{0.0f, 0.05f, 0.0f});
    o->addVertex(Vertex3f{0.0f, scale, 0.0f});
    AddObject(std::move(o));

    o = std::make_unique<LineObject>(PaintColoring::CColor::White, PaintColoring::CColor::Magenta);
    o->name = "-Y";
    o->addVertex(Vertex3f{0.0f, 0.05f, 0.0f});
    o->addVertex(Vertex3f{0.0f, -scale, 0.0f});
    AddObject(std::move(o));
}

void Database::AddTetrahedron(float px, float py, float pz,
                              float height, float baseScale, bool animate,
                              const PaintColoring::CColor &color)
{
    Vertex3f p1{};
    Triangle t;

    std::unique_ptr<WireMeshObject> o = std::make_unique<WireMeshObject>();
    o->Initialize(PaintColoring::CColor::White, color, 8);
    o->animate = animate;
    o->name = "Tetrahedron"; // Pyramid
    o->setPosition(px, py, pz);
    o->SetType(Object3D::ObjectType::WireMesh);

    p1.set(-0.5f * baseScale, 0.0f, 0.5f * baseScale); // 1
    o->AddVertex(p1);
    p1.set(0.5f * baseScale, 0.0f, 0.5f * baseScale); // 2
    o->AddVertex(p1);
    p1.set(0.0f, 0.0f, -0.5f * baseScale); // 3
    o->AddVertex(p1);
    p1.set(0.0f, height * baseScale, 0.0f); // 4
    o->AddVertex(p1);

    // 6 edges all visible
    // 1 - 12(0-11)
    for (int i = 0; i < 6; i++)
        o->AddEdge(EdgeProperty::NONPERMINENT, EdgeProperty::VISIBLE);

    // Side face
    o->AddTriangle(1, 2, 4);
    t = o->GetTriangle();
    t.SetEdgeIndex(1, 1);
    t.SetEdgeIndex(2, 4);
    t.SetEdgeIndex(3, 6);

    // Side face
    o->AddTriangle(2, 3, 4);
    t.SetEdgeIndex(1, 2);
    t.SetEdgeIndex(2, 5);
    t.SetEdgeIndex(3, 4);

    // Side face
    o->AddTriangle(3, 1, 4);
    t.SetEdgeIndex(1, 3);
    t.SetEdgeIndex(2, 6);
    t.SetEdgeIndex(3, 5);

    // Bottom face
    o->AddTriangle(1, 3, 2);
    t.SetEdgeIndex(1, 3);
    t.SetEdgeIndex(2, 2);
    t.SetEdgeIndex(3, 1);

    AddObject(std::move(o));
}

// NOT COMPLETE YET
void Database::AddPlane(float width, float height, float px, float py, float pz, int r, int g, int b)
{
    std::unique_ptr<Object3D> o;

    o = std::make_unique<WireMeshObject>();
    o->name = "Plane"; // +Z

    o->setPosition(px, py, pz);
    // o->setOrientation(-90.0f, 0.0f, 1.0f, 0.0f);

    Vertex3f p1{};

    p1.set(-width / 2, -height / 2, 0.0f); // 1
    o->addVertex(p1);
    p1.set(width / 2, -height / 2, 0.0f); // 2
    o->addVertex(p1);
    p1.set(width / 2, height / 2, 0.0f); // 3
    o->addVertex(p1);
    p1.set(-width / 2, height / 2, 0.0f); // 4
    o->addVertex(p1);
}
