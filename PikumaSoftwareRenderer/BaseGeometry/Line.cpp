#include "Line.h"
#include <sstream>

namespace Geometry
{
    Line::Line(/* args */)
    {
    }

    Line::Line(const Maths::Vector4f &p1, const Maths::Vector4f &p2, CColor color)
    {
        points[0] = p1;
        points[1] = p2;
        this->color = color;
    }

    Line::~Line()
    {
    }

    void Line::set(const Line &line)
    {
        points[0].set(line.points[0]);
        points[1].set(line.points[1]);
        color = line.color;
    }

    void Line::set(const Maths::Vector4f &p1, const Maths::Vector4f &p2, CColor color)
    {
        points[0].set(p1);
        points[1].set(p2);
        this->color = color;
    }

    void Line::print() const
    {
        std::cout << *this << std::endl;
    }

    std::string Line::toString() const
    {
        std::ostringstream oss;
        oss << *this;

        return oss.str();
    }

} // namespace Geometry
