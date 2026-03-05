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

void Database::AddTripod(float scale)
{
    std::unique_ptr<Object3D> o;

    o = std::make_unique<LineObject>(PaintColoring::CColor::BgDarkGray, PaintColoring::CColor::Red);
    o->name = "+X";
    o->addVertex(Vertex3f{0.0f, 0.05f, 0.0f});
    o->addVertex(Vertex3f{scale, 0.05f, 0.0f});
    AddObject(std::move(o));

    o = std::make_unique<LineObject>(PaintColoring::CColor::BgDarkGray, PaintColoring::CColor::Yellow);
    o->name = "-X";
    o->addVertex(Vertex3f{0.0f, 0.05f, 0.0f});
    o->addVertex(Vertex3f{-scale, 0.05f, 0.0f});
    AddObject(std::move(o));

    o = std::make_unique<LineObject>(PaintColoring::CColor::BgDarkGray, PaintColoring::CColor::Green);
    o->name = "+Z";
    o->addVertex(Vertex3f{0.0f, 0.05f, 0.0f});
    o->addVertex(Vertex3f{0.0f, 0.05f, scale});
    AddObject(std::move(o));

    o = std::make_unique<LineObject>(PaintColoring::CColor::BgDarkGray, PaintColoring::CColor::Black);
    o->name = "-Z";
    o->addVertex(Vertex3f{0.0f, 0.05f, 0.0f});
    o->addVertex(Vertex3f{0.0f, 0.05f, -scale});
    AddObject(std::move(o));

    o = std::make_unique<LineObject>(PaintColoring::CColor::BgDarkGray, PaintColoring::CColor::Blue);
    o->name = "+Y";
    o->addVertex(Vertex3f{0.0f, 0.05f, 0.0f});
    o->addVertex(Vertex3f{0.0f, scale, 0.0f});
    AddObject(std::move(o));

    o = std::make_unique<LineObject>(PaintColoring::CColor::BgDarkGray, PaintColoring::CColor::Magenta);
    o->name = "-Y";
    o->addVertex(Vertex3f{0.0f, 0.05f, 0.0f});
    o->addVertex(Vertex3f{0.0f, -scale, 0.0f});
    AddObject(std::move(o));
}
