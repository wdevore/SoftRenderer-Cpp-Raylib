#pragma once

#include "LineCollection.h"

namespace Geometry
{
    class TriAxisLines : public LineCollection
    {
    public:
        TriAxisLines(/* args */);
        ~TriAxisLines();

        void build() override;
    };

} // namespace Geometry
