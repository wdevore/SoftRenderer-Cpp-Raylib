#include "TriAxisLines.h"

namespace Geometry
{

    TriAxisLines::TriAxisLines(/* args */)
    {
    }

    TriAxisLines::~TriAxisLines()
    {
    }

    void TriAxisLines::build()
    {
        Geometry::Line lineXAxis{Maths::Vector4f{-2.0f, 0.0f, 0.0f}, Maths::Vector4f{2.0f, 0.0f, 0.0f}, CColor::Red};
        lineXAxis.translation.set(5, 0.0, 0);
        lineXAxis.scale.set(1, 1, 1);
        // +Angle = CCW rotation
        // lineXAxis.rotation.set(0, 0, 45.0 * Maths::DEGTORAD);
        addLine(lineXAxis);

        Geometry::Line lineYAxis{Maths::Vector4f{0.0f, -2.0f, 0.0f}, Maths::Vector4f{0.0f, 2.0f, 0.0f}, CColor::Green};
        lineYAxis.translation.set(5, 0.0, 0);
        lineYAxis.scale.set(1, 1, 1);
        addLine(lineYAxis);

        Geometry::Line lineZAxis{Maths::Vector4f{0.0f, 0.0f, -2.0f}, Maths::Vector4f{0.0f, 0.0f, 2.0f}, CColor::Orange};
        lineZAxis.translation.set(5, 0.0, 0);
        lineZAxis.scale.set(1, 1, 1);
        addLine(lineZAxis);
    }

}