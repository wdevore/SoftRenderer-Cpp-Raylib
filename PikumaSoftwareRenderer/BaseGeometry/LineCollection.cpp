#include "LineCollection.h"

namespace Geometry
{
    LineCollection::LineCollection(/* args */)
    {
    }

    LineCollection::~LineCollection()
    {
    }

    void LineCollection::buildLineGrid()
    {
        for (float i = 0; i < 5; i++)
        {
            for (float j = 0; j < 5; j++)
            {
                Line line{Maths::Vector4f{i, j, 0.0f}, Maths::Vector4f{i, j, 0.0f}, CColor::White};
                lines.push_back(line);
            }
        }
    }
}