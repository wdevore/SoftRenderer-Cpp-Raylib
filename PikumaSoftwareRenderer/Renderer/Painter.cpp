#include "Painter.h"

Painter::~Painter()
{
}

void Painter::DrawZLine(Canvas &canvas, ZBuffer &zb,
                        Vector3f &v0, Vector3f &v1,
                        CColor color)
{
    DrawZBresenhamLine(canvas, zb, v0.x, v0.y, v1.x, v1.y, v0.z, v1.z, color);
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
