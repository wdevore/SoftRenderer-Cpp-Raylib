#pragma once

#include "LineCollection.h"

namespace Geometry
{
    class GridLines : public LineCollection
    {
    private:
        /* data */
    public:
        GridLines(/* args */);
        ~GridLines();

        void build() override;
    };

} // namespace Geometry
