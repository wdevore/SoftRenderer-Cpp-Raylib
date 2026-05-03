#pragma once

#include "Canvas.h" // Includes Raylib
#include "CColor.h"
#include "ZBuffer.h"
#include "Vectorf.h"

class Painter
{
private:
    int width{};
    int height{};

    Color ca{};

    // Z buffer variables
    float zrP{};
    float zrQ{};
    float dzr{};
    float dx{};
    float dy{};
    float z{};
    float dzdx{};
    float dzdy{};
    int zl{};
    int zstatus{};

    // Bresenham Line algorithm variables
    int x{};
    int y{};
    int C{};
    int D{};
    int M{};
    int HX{};
    int HY{};
    int xInc{};
    int yInc{};
    float I{};

public:
    Painter() {};
    ~Painter();

    void Initialize(int width, int height)
    {
        this->width = width;
        this->height = height;
    }

    void DrawBresenhamLine(Canvas &canvas,
                           int xP, int yP, int xQ, int yQ,
                           CColor &color);
    void DrawDDALine(Canvas &canvas,
                     float x0, float y0, float x1, float y1,
                     CColor &color);

    void DrawZLine(Canvas &canvas, ZBuffer &zb, Vectorf &v0, Vectorf &v1, CColor color);

    void DrawZBresenhamLine(Canvas &canvas, ZBuffer &zb,
                            int xP, int yP, int xQ, int yQ, float zP, float zQ,
                            CColor &color);

    void DrawGrid(Canvas &canvas, CColor &color);
    void DrawDottedGrid(Canvas &canvas, CColor &color);
    void DrawRectangle(Canvas &canvas, int x, int y, int width, int height, CColor &color);
    void DrawTriangleWire(Canvas &canvas, int v0x, int v0y, int v1x, int v1y, int v2x, int v2y, CColor &color);
};
