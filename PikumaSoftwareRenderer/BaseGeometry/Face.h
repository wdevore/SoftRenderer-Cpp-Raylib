#pragma once

#include <cstdint>

#include <Vector3f.h>
#include <Texture2f.h>

namespace Geometry
{
    class Face
    {
    private:
    public:
        int a;
        int b;
        int c;
        Maths::Texture2f a_uv;
        Maths::Texture2f b_uv;
        Maths::Texture2f c_uv;
        uint32_t color;
    };
} // namespace Geometry
