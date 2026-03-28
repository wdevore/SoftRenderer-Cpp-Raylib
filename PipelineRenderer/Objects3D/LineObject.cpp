#include "LineObject.h"

LineObject::LineObject(/* args */)
{
}

LineObject::~LineObject()
{
}

void LineObject::Build(std::vector<Vector3f> &vertices)
{
    // Get the current offset where these vertices are placed
    // within the collection
    o = vertices.size();
    float baseScale = 1.0f;
    type = ObjectType::Line;

    // 2 vertices (+X axis)
    vertices.push_back(Vector3f{0.0f * baseScale, 0.0f, 1.0f * baseScale}); // 0
    vertices.push_back(Vector3f{0.5f * baseScale, 0.0f, 1.0f * baseScale}); // 1
}
