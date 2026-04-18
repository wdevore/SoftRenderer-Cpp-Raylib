#pragma once

#include "Canvas.h" // Includes Raylib
#include "CColor.h"
#include "ZBuffer.h"
#include "Vector3f.h"

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

    void DrawZLine(Canvas &canvas, ZBuffer &zb, Vector3f &v0, Vector3f &v1, CColor color);

    void DrawZBresenhamLine(Canvas &canvas, ZBuffer &zb,
                            int xP, int yP, int xQ, int yQ, float zP, float zQ,
                            CColor &color);

    void DrawGrid(Canvas &canvas, CColor &color);
    void DrawRectangle(Canvas &canvas, int x, int y, int width, int height, CColor &color);
};
