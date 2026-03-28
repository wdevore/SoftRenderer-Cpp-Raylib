#include "CubeObject.h"

CubeObject::CubeObject(/* args */)
{
}

CubeObject::~CubeObject()
{
}

void CubeObject::Build(std::vector<Vector3f> &vertices)
{
    // Get the current offset where these vertices are placed
    // within the collection
    o = vertices.size();

    // 8 vertices
    vertices.push_back(Vector3f{-0.5f, -0.5f, 0.5f});  // 0: Bottom-front-left
    vertices.push_back(Vector3f{0.5f, -0.5f, 0.5f});   // 1: Bottom-front-right
    vertices.push_back(Vector3f{0.5f, 0.5f, 0.5f});    // 2: Top-front-right
    vertices.push_back(Vector3f{-0.5f, 0.5f, 0.5f});   // 3: Top-front-left
    vertices.push_back(Vector3f{-0.5f, -0.5f, -0.5f}); // 4: Bottom-back-left
    vertices.push_back(Vector3f{0.5f, -0.5f, -0.5f});  // 5: Bottom-back-right
    vertices.push_back(Vector3f{0.5f, 0.5f, -0.5f});   // 6: Top-back-right
    vertices.push_back(Vector3f{-0.5f, 0.5f, -0.5f});  // 7: Top-back-left

    // Define the faces relative to the offset and are indices into
    // the vertices collection.

    faces.resize(12);

    // Front face
    faces[0].Set(o + 0, o + 1, o + 2);
    CalcFaceNormal(vertices, faces[0]);
    faces[1].Set(o + 0, o + 2, o + 3);
    CalcFaceNormal(vertices, faces[1]);
    // Back face
    faces[2].Set(o + 5, o + 4, o + 7);
    CalcFaceNormal(vertices, faces[2]);
    faces[3].Set(o + 5, o + 7, o + 6);
    CalcFaceNormal(vertices, faces[3]);
    // Top face
    faces[4].Set(o + 3, o + 2, o + 6);
    CalcFaceNormal(vertices, faces[4]);
    faces[5].Set(o + 3, o + 6, o + 7);
    CalcFaceNormal(vertices, faces[5]);
    // Bottom face
    faces[6].Set(o + 4, o + 5, o + 1);
    CalcFaceNormal(vertices, faces[6]);
    faces[7].Set(o + 4, o + 1, o + 0);
    CalcFaceNormal(vertices, faces[7]);
    // Right face
    faces[8].Set(o + 1, o + 5, o + 6);
    CalcFaceNormal(vertices, faces[8]);
    faces[9].Set(o + 1, o + 6, o + 2);
    CalcFaceNormal(vertices, faces[9]);
    // Left face
    faces[10].Set(o + 4, o + 0, o + 3);
    CalcFaceNormal(vertices, faces[10]);
    faces[11].Set(o + 4, o + 3, o + 7);
    CalcFaceNormal(vertices, faces[11]);
}
