#include <cmath>

#include "Painter.h"
#include "Vector4f.h"
#include "Vector2f.h"

Painter::~Painter()
{
}

void Painter::reset()
{
    zb.reset();
}

void Painter::DrawBresenhamLine(Canvas &canvas, int xP, int yP, int xQ, int yQ, CColor &color)
{
    xP = std::max(0, std::min(xP, width - 1));
    yP = std::max(0, std::min(yP, height - 1));

    ca.r = color.r;
    ca.g = color.g;
    ca.b = color.b;
    ca.a = color.a;

    x = xP;
    y = yP;
    D = 0;
    HX = xQ - xP;
    HY = yQ - yP;
    xInc = 1;
    yInc = 1;
    if (HX < 0)
    {
        xInc = -1;
        dzdx = -dzdx;
        HX = -HX;
    }
    if (HY < 0)
    {
        yInc = -1;
        dzdy = -dzdy;
        HY = -HY;
    }
    if (HY <= HX)
    {
        C = 2 * HX;
        M = 2 * HY;
        for (;;)
        {
            canvas.PutPixel(x, y, ca);
            if (x == xQ)
                break;
            x += xInc;
            D += M;
            if (D > HX)
            {
                y += yInc;
                D -= C;
            }
        }
    }
    else
    {
        C = 2 * HY;
        M = 2 * HX;
        for (;;)
        {
            canvas.PutPixel(x, y, ca);
            if (y == yQ)
                break;
            y += yInc;
            z += dzdy;
            D += M;
            if (D > HY)
            {
                x += xInc;
                D -= C;
            }
        }
    }
}

void Painter::DrawDDALine(Canvas &canvas, float x0, float y0, float x1, float y1, CColor &color)
{
    x0 = std::fmax(0, std::fmin(x0, width - 1));
    y0 = std::fmax(0, std::fmin(y0, height - 1));
    x1 = std::fmax(0, std::fmin(x1, width - 1));
    y1 = std::fmax(0, std::fmin(y1, height - 1));

    float delta_x = (x1 - x0);
    float delta_y = (y1 - y0);

    float longest_side_length = (std::fabs(delta_x) >= std::fabs(delta_y)) ? std::fabs(delta_x) : std::fabs(delta_y);

    float x_inc = delta_x / longest_side_length;
    float y_inc = delta_y / longest_side_length;

    float current_x = x0;
    float current_y = y0;

    ca.r = color.r;
    ca.g = color.g;
    ca.b = color.b;
    ca.a = color.a;

    for (int i = 0; i <= longest_side_length; i++)
    {
        canvas.PutPixel(round(current_x), round(current_y), ca);
        current_x += x_inc;
        current_y += y_inc;
    }
}

void Painter::DrawDDAZLine(Canvas &canvas, float x0, float y0, float x1, float y1, float zP, float zQ, CColor &color)
{
    float delta_x = (x1 - x0);
    float delta_y = (y1 - y0);

    float longest_side_length = (std::fabs(delta_x) >= std::fabs(delta_y)) ? std::fabs(delta_x) : std::fabs(delta_y);

    // Prevent division by zero if both points are exactly the same
    if (longest_side_length == 0)
        return;

    float x_inc = delta_x / longest_side_length;
    float y_inc = delta_y / longest_side_length;
    float z_inc = (zQ - zP) / longest_side_length;

    float current_x = x0;
    float current_y = y0;
    float current_z = zP;

    ca.r = color.r;
    ca.g = color.g;
    ca.b = color.b;
    ca.a = color.a;

    for (int i = 0; i <= longest_side_length; i++)
    {
        int cx = std::round(current_x);
        int cy = std::round(current_y);

        // Scissor out-of-bounds pixels, but perform depth testing and drawing for on-screen pixels
        if (cx >= 0 && cx < width && cy >= 0 && cy < height)
        {
            if (current_z < zb.getZ(cx, cy))
            {
                canvas.PutPixel(cx, cy, ca);
                zb.setZ(cx, cy, current_z);
            }
        }

        current_x += x_inc;
        current_y += y_inc;
        current_z += z_inc;
    }
}

void Painter::DrawZLine(Canvas &canvas,
                        float v0x, float v0y, float v1x, float v1y, float v0z, float v1z,
                        CColor color)
{
    DrawDDAZLine(canvas,
                 v0x, v0y,
                 v1x, v1y,
                 v0z, v1z,
                 color);
}

void Painter::DrawZBresenhamLine(Canvas &canvas, ZBuffer &zb,
                                 int xP, int yP, int xQ, int yQ,
                                 float zP, float zQ,
                                 CColor &color)
{
    if (xP < 0 || xP > width - 1 || xQ < 0 || xQ > width - 1)
        return;
    if (yP < 0 || yP > height - 1 || yQ < 0 || yQ > height - 1)
        return;

    ca.r = color.r;
    ca.g = color.g;
    ca.b = color.b;
    ca.a = color.a;

    zrP = 1.0f / zP;
    zrQ = 1.0f / zQ;
    dzr = zrQ - zrP;
    dx = (xQ - xP);
    dy = (yQ - yP);
    z = zrP;
    dzdx = dzr / dx;
    dzdy = dzr / dy;

    x = xP;
    y = yP;
    D = 0;
    HX = xQ - xP;
    HY = yQ - yP;
    xInc = 1;
    yInc = 1;
    if (HX < 0)
    {
        xInc = -1;
        dzdx = -dzdx;
        HX = -HX;
    }
    if (HY < 0)
    {
        yInc = -1;
        dzdy = -dzdy;
        HY = -HY;
    }
    if (HY <= HX)
    {
        C = 2 * HX;
        M = 2 * HY;
        for (;;)
        {
            zl = zb.getIndex(x, y);
            if (zl < 0)
                break; // pixel off screen
            zstatus = zb.setZ(zl, z, false);
            if (zstatus == 1)
            {
                canvas.PutPixel(x, y, ca); // pixel closer
            }
            if (x == xQ)
                break;
            x += xInc;
            z += dzdx;
            D += M;
            if (D > HX)
            {
                y += yInc;
                D -= C;
            }
        }
    }
    else
    {
        C = 2 * HY;
        M = 2 * HX;
        for (;;)
        {
            zl = zb.getIndex(x, y);
            if (zl < 0)
                break; // pixel off screen
            zstatus = zb.setZ(zl, z, false);
            if (zstatus == 1)
            {
                canvas.PutPixel(x, y, ca); // pixel closer
            }
            if (y == yQ)
                break;
            y += yInc;
            z += dzdy;
            D += M;
            if (D > HY)
            {
                x += xInc;
                D -= C;
            }
        }
    }
}

void Painter::DrawGrid(Canvas &canvas, CColor &color)
{
    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            if (x % 10 == 0 || y % 10 == 0)
            {
                canvas.PutPixel(x, y, color);
            }
        }
    }
}

void Painter::DrawDottedGrid(Canvas &canvas, CColor &color)
{
    for (int y = 0; y < height; y += 10)
    {
        for (int x = 0; x < width; x += 10)
        {
            canvas.PutPixel(x, y, color);
        }
    }
}

void Painter::DrawRectangle(Canvas &canvas, int x, int y, int width, int height, CColor &color)
{
    for (int i = 0; i < width; i++)
    {
        for (int j = 0; j < height; j++)
        {
            canvas.PutPixel(x + i, y + j, color);
        }
    }
}

void Painter::DrawTriangleWire(Canvas &canvas, int v0x, int v0y, int v1x, int v1y, int v2x, int v2y, CColor &color)
{
    DrawDDALine(canvas, v0x, v0y, v1x, v1y, color);
    DrawDDALine(canvas, v1x, v1y, v2x, v2y, color);
    DrawDDALine(canvas, v2x, v2y, v0x, v0y, color);
}

void Painter::DrawFilledTriangle(Canvas &canvas, const Geometry::Triangle &triangle, CColor &color)
{
    const Maths::Vector4f *a = &triangle.points[0]; // v0
    const Maths::Vector4f *b = &triangle.points[1]; // v1
    const Maths::Vector4f *c = &triangle.points[2]; // v2
    // const Maths::Vector4f *a = new Maths::Vector4f{163.347809, 126.778587, 0.0f};
    // const Maths::Vector4f *b = new Maths::Vector4f{179.263596, 128.011566, 0.0f};
    // const Maths::Vector4f *c = new Maths::Vector4f{163.41626, 111.056435, 0.0f};

    // Finds the bounding box with all candidate pixels
    int x_min = std::floor(std::fmin(std::fmin(a->x, b->x), c->x));
    int y_min = std::floor(std::fmin(std::fmin(a->y, b->y), c->y));
    int x_max = ceil(std::fmax(std::fmax(a->x, b->x), c->x));
    int y_max = ceil(std::fmax(std::fmax(a->y, b->y), c->y));

    // Screen 2D points from vertices v0, v1, and v2
    Maths::Vector2f sv0{a->x, a->y};
    Maths::Vector2f sv1{b->x, b->y};
    Maths::Vector2f sv2{c->x, c->y};

    // Compute the area of the entire triangle/parallelogram
    float area = Maths::area(sv0, sv1, sv2);

    // Optional cull:
    // Back-face culling using the signed-area
    // if (area <= 0)
    // {
    //     return;
    // }

    // Compute the constant deltas that will be used for the horizontal and vertical steps
    float delta_w0_col = (b->y - c->y);
    float delta_w1_col = (c->y - a->y);
    float delta_w2_col = (a->y - b->y);
    float delta_w0_row = (c->x - b->x);
    float delta_w1_row = (a->x - c->x);
    float delta_w2_row = (b->x - a->x);

    // Compute the edge functions for the fist (top-left) point
    Maths::Vector2f p0{x_min + 0.5f, y_min + 0.5f};
    float w0_row = Maths::area(sv1, sv2, p0);
    float w1_row = Maths::area(sv2, sv0, p0);
    float w2_row = Maths::area(sv0, sv1, p0);

    // Loop all candidate pixels inside the bounding box
    for (int y = y_min; y <= y_max; y++)
    {
        float w0 = w0_row;
        float w1 = w1_row;
        float w2 = w2_row;
        for (int x = x_min; x <= x_max; x++)
        {
            bool is_inside = area > 0 ? (w0 >= 0 && w1 >= 0 && w2 >= 0) : (w0 <= 0 && w1 <= 0 && w2 <= 0);
            if (is_inside)
            {
                float alpha = w0 / area;
                float beta = w1 / area;
                float gamma = w2 / area;

                // Interpolate the value of 1/w for the current pixel
                float interpolated_reciprocal_w = (1 / a->w) * alpha + (1 / b->w) * beta + (1 / c->w) * gamma;

                // Adjust 1/w so the pixels that are closer to the camera have smaller values
                interpolated_reciprocal_w = 1.0 - interpolated_reciprocal_w;

                // Only draw the pixel if the depth value is less than the one previously stored in the z-buffer
                if (interpolated_reciprocal_w < zb.getZ(x, y))
                {
                    // Draw a pixel at position (x,y) with a solid color
                    ca.r = color.r;
                    ca.g = color.g;
                    ca.b = color.b;
                    ca.a = color.a;

                    canvas.PutPixel(x, y, ca);

                    // Update the z-buffer value with the 1/w of this current pixel
                    zb.setZ(x, y, interpolated_reciprocal_w);
                }
            }
            w0 += delta_w0_col;
            w1 += delta_w1_col;
            w2 += delta_w2_col;
        }
        w0_row += delta_w0_row;
        w1_row += delta_w1_row;
        w2_row += delta_w2_row;
    }
}
