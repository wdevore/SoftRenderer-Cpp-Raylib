#pragma once

#include "upng.h"

namespace Geometry
{
    class Texture
    {
    private:
        /* data */
    public:
        upng_t *png_image{};

        Texture(/* args */);
        ~Texture();
    };

} // namespace Geometry
