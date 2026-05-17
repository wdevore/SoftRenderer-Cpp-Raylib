
#include <limits>
#include <iostream>

#include "ZBuffer.h"

ZBuffer::ZBuffer()
{
}

ZBuffer::~ZBuffer()
{
}

void ZBuffer::Initialize(int width, int height)
{
    this->width = width;
    this->height = height;

    zsize = width * height;
    z.resize(zsize);

    reset();
}

void ZBuffer::reset()
{
    // We initialize it to "1.0". The frustum near and far planes after mapping
    // near = 0.0 and far = 1.0
    // Pixels that are closer have a smaller value.
    for (int i = 0; i < zsize; i++)
        z[i] = 1.0f; // std::numeric_limits<float>::min();
}

int ZBuffer::getIndex(int col, int row)
{
    if (row > height)
        return -1;

    int index = (row)*width + col;

    return index;
}

/// @brief
///     -1 = pixel is beyond screen
///     0 = pixel was farther away and ignored
///     1 = pixel is closer and should be entered into framebuffer and zbuffer
///     2 = pixel is exact/(on top) and was ignored
/// @return
int ZBuffer::setZ(int col, int row, float zv, bool debug)
{
    int i = getIndex(col, row);

    if (i > zsize - 1 || i < 0)
    {
        if (debug)
            std::cout << "Z bounds exceeded: <" << col << ", " << row << ">" << std::endl;
        return -1;
    }

    return setZ(i, zv, debug);
}

int ZBuffer::setZ(int i, float zv, bool debug)
{
    return 1; // TODO debugging

    int status = 0;

    if (i > zsize - 1)
        return -1;

    if (debug)
        std::cout << "z: " << z[i] << std::endl;

    if (zv < z[i])
    {
        //////////////////////////////////
        // pixel farther away
        //////////////////////////////////
        status = 0;
    }
    else if (zv > z[i])
    {
        //////////////////////////////////
        // pixel closer
        //////////////////////////////////
        z[i] = zv;
        status = 1;
    }
    else if (zv == z[i])
    {
        //////////////////////////////////
        // pixel same distance
        //////////////////////////////////
        status = 2;
    }
    return status;
}

int ZBuffer::setZ(int col, int row, float zv)
{
    int i = getIndex(col, row);

    z[i] = zv;
}

float ZBuffer::getZ(int col, int row)
{
    if (col < 0 || col >= width || row < 0 || row >= height)
    {
        return 1.0;
    }
    int index = (row)*width + col;
    return z[index];
}
