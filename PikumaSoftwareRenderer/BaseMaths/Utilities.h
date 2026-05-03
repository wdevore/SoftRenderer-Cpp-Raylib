#pragma once

namespace Utilities
{
    float floatLerp(float a, float b, float t)
    {
        return a + t * (b - a);
    }
} // namespace Utilities
