#pragma once

#include "raylib.h"

#include "EdgeInterpolation.h"
#include "GradientInterpolation.h"
#include "WuColor.h"
#include "CColor.h"
#include "ZBuffer.h"
#include "Canvas.h"

class Painting
{
private:
    int width;
    int height;

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

    // Wu algorithm variables
    int DeltaX{};
    int DeltaY{};
    int Temp{};
    int XDir{};
    int IntensityShift{};
    int ErrorAccTemp{};
    int ErrorAdj{};
    int ErrorAcc{};
    int Weighting{};
    int WeightingComplementMask{};
    Color colorWeighting{};
    Color cc{};
    Color cp{};
    Color ca{};

    // Z variables
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
    float Tempf{};

    // Scanline algorithm variables
    EdgeInterpolation Left{};
    EdgeInterpolation Right{};

    // Z buffer vars
    ZBuffer zb{};

public:
    Painting();
    ~Painting();

    void Initialize(int width, int height);
    void Update();

    // Primitives
    void DrawBresenhamLine(Canvas &canvas, int xP, int yP, int xQ, int yQ, PaintColoring::CColor color);
    void DrawZBresenhamLine(Canvas &canvas, int xP, int yP, int xQ, int yQ, float zP, float zQ, PaintColoring::CColor color);

    // Wu algorithm
    void DrawWuIndexedLine(Canvas &canvas, int X0, int Y0, int X1, int Y1, PaintColoring::WuColor color);
    void DrawWuBlendedLine(Canvas &canvas, int X0, int Y0, int X1, int Y1, PaintColoring::WuColor color);
    void DrawZWuBlendedLine(Canvas &canvas, int X0, int Y0, int X1, int Y1, float zP, float zQ, PaintColoring::WuColor color);

    // Scanline algorithms: Triangle rasterization
    long DrawFlatTriangle(Canvas &canvas, GradientInterpolation &g,
                          EdgeInterpolation &TM, EdgeInterpolation &TB, EdgeInterpolation &MB,
                          bool blnMiddleIsLeft,
                          PaintColoring::CColor &color);
    void DrawZFlatScanLine(Canvas &canvas, GradientInterpolation &g,
                           EdgeInterpolation &l, EdgeInterpolation &r,
                           PaintColoring::CColor &color);
};
