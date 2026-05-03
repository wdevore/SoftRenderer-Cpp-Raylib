#pragma once

#include <cstdint>

#include "Vector3f.h"

namespace Lights
{
    class DirectionalLight
    {
    private:
        /* data */
    public:
        Maths::Vector3f direction;

        DirectionalLight(/* args */);
        ~DirectionalLight();

        uint32_t applyLightIntensity(uint32_t original_color, float factor);
    };

} // namespace Lights
