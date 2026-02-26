#pragma once
#include "raylib.h"

#include <vector>

#include "WuColor.h"
#include "ZBuffer.h"

class Canvas
{
public:
    Canvas(int width, int height);
    ~Canvas();

    void initialize();
    void Clear();
    void SetClearColor(Color c);
    void PutPixel(int x, int y, Color c);
    Color GetPixel(int x, int y);
    void Update();
    void Blit(int x, int y);

    // Primitives
    void DrawBresenhamLine(int xP, int yP, int xQ, int yQ, Color c);
    void DrawZBresenhamLine(int xP, int yP, int xQ, int yQ, float zP, float zQ, Color c);
    void DrawWuIndexedLine(int X0, int Y0, int X1, int Y1, WuColor color);
    void DrawWuBlendedLine(int X0, int Y0, int X1, int Y1, WuColor color);

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

    // Z buffer vars
    ZBuffer zb{};

    Image canvas;
    Texture2D targetTexture;

    Color clearColor = WHITE;
};