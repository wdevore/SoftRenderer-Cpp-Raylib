#include "GridLines.h"

namespace Geometry
{
    GridLines::GridLines(/* args */)
    {
    }

    GridLines::~GridLines()
    {
    }

    /// @brief Builds a grid on the X,Z plane
    void GridLines::build()
    {
        float length = 10.0f;

        // First add the Z axis line
        Line lineZ{Maths::Vector4f{0, 0, -length}, Maths::Vector4f{0, 0, length}, CColor::LighterGray};
        lineZ.translation.set(0, 0.0, 0);
        addLine(lineZ);

        // Next add the X axis line
        Line lineX{Maths::Vector4f{-length, 0, 0}, Maths::Vector4f{length, 0, 0}, CColor::LighterGray};
        lineX.translation.set(0, 0.0, 0);
        addLine(lineX);

        float stepSize = 1.0f;
        float numberOfLines = 5 + 5;

        // Z lines march along the X axis.
        for (float z = stepSize; z < numberOfLines + 1; z += stepSize)
        {
            Line line{Maths::Vector4f{-length, 0, 0}, Maths::Vector4f{length, 0, 0}, CColor::LightGray};
            line.translation.set(0, 0.0, z);
            addLine(line);
        }

        for (float z = -stepSize; z > -numberOfLines - 1; z += -stepSize)
        {
            Line line{Maths::Vector4f{-length, 0, 0}, Maths::Vector4f{length, 0, 0}, CColor::LightGray};
            line.translation.set(0, 0.0, z);
            addLine(line);
        }

        // X lines march along the Z axis.
        for (float x = stepSize; x < numberOfLines + 1; x += stepSize)
        {
            Line line{Maths::Vector4f{0, 0, -length}, Maths::Vector4f{0, 0, length}, CColor::LightGray};
            line.translation.set(x, 0.0, 0);
            addLine(line);
        }

        for (float x = -stepSize; x > -numberOfLines - 1; x += -stepSize)
        {
            Line line{Maths::Vector4f{0, 0, -length}, Maths::Vector4f{0, 0, length}, CColor::LightGray};
            line.translation.set(x, 0.0, 0);
            addLine(line);
        }
    }

} // namespace Geometry