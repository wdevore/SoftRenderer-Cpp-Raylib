#pragma once
#include "raylib.h"

#include <vector>

#include "CColor.h"

class Canvas
{
public:
    Canvas();
    ~Canvas();

    Color c{};

    void initialize(int width, int height);
    void Clear();
    void SetClearColor(Color c);
    void PutPixel(int x, int y, Color c);
    void PutPixel(int x, int y, CColor &c);
    Color GetPixel(int x, int y);
    void Update();
    void Blit(int x, int y);

private:
    int width;
    int height;

    Image canvas;
    Texture2D targetTexture;

    Color clearColor = WHITE;
};