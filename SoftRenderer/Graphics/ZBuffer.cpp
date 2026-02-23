
#include <limits>
#include <iostream>

#include "ZBuffer.h"

ZBuffer::ZBuffer(int width, int height)
{
    this->width = width;
    this->height = height;
}

ZBuffer::~ZBuffer()
{
}

void ZBuffer::initialize()
{
    zsize = width * height;
    z.resize(zsize);
    reset();
}

void ZBuffer::reset()
{
    // We initialize it to "infinity" (or a very large number) so that any
    // incoming geometry is closer than the background.
    for (int i = 0; i < zsize; i++)
        z[i] = std::numeric_limits<float>::max();
}

int ZBuffer::getIndex(int col, int row)
{
    if (row > height)
        return -1;

    int index = (row - 1) * width + col;

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
