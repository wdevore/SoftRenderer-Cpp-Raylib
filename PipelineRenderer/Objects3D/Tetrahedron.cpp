#include "Tetrahedron.h"

Tetrahedron::Tetrahedron(/* args */)
{
}

Tetrahedron::~Tetrahedron()
{
}

void Tetrahedron::Build(std::vector<Vector3f> &vertices)
{
    // Get the current offset where these vertices are placed
    // within the collection
    o = vertices.size();
    float baseScale = 1.0f;
    float height = 0.5f;
    type = ObjectType::WireMesh;

    // 8 vertices
    vertices.push_back(Vector3f{-0.2f, 0.0f, 0.0f});  // 0: Bottom-left
    vertices.push_back(Vector3f{0.2f, 0.0f, 0.0f});   // 1: Bottom-right
    vertices.push_back(Vector3f{0.0f, 0.0f, -0.5f});  // 2: Bottom-far
    vertices.push_back(Vector3f{0.0f, height, 1.0f}); // 3: Top

    // Define the faces relative to the offset and are indices into
    // the vertices collection.

    faces.resize(4);

    // Side face
    faces[0].Set(o + 0, o + 1, o + 3);
    CalcFaceNormal(vertices, faces[0]);
    // Side face
    faces[1].Set(o + 1, o + 2, o + 3);
    CalcFaceNormal(vertices, faces[1]);
    // Side face
    faces[2].Set(o + 3, o + 0, o + 3);
    CalcFaceNormal(vertices, faces[2]);
    // Bottom face
    faces[3].Set(o + 0, o + 2, o + 1);
    CalcFaceNormal(vertices, faces[3]);
}
