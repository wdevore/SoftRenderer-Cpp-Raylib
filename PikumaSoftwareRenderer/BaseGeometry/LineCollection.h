#pragma once

#include <vector>

#include "LineCollection.h"
#include "Line.h"
#include "Vector4f.h"

namespace Geometry
{
    class LineCollection
    {
    private:
        /* data */
    public:
        std::vector<Line> lines;

        LineCollection(/* args */);
        ~LineCollection();

        void addLine(const Line line)
        {
            lines.push_back(line);
        }

        virtual void build() = 0;
    };

} // namespace Geometry
