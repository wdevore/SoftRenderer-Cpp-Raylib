#include "Database.h"
#include "WireMeshObject.h"

Database::Database(/* args */)
{
}

Database::~Database()
{
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
