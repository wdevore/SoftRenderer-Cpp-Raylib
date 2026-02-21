#pragma once
#include "raylib.h"
#include <vector>

class Canvas
{
public:
    Canvas(int width, int height);
    ~Canvas();

    void Clear();
    void SetClearColor(Color c);
    void PutPixel(int x, int y, Color c);
    void Update();
    void Blit(int x, int y);

private:
    int width;
    int height;

    Image canvas;
    Texture2D targetTexture;

    Color clearColor = WHITE;
};