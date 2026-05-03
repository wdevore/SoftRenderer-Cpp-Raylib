#include "Texture.h"

namespace Geometry
{
    Texture::Texture(/* args */)
    {
    }

    Texture::~Texture()
    {
        if (png_image != nullptr)
        {
            upng_free(png_image);
        }
    }

} // namespace Geometry
