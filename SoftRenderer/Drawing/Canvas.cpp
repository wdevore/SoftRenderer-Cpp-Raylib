#include "Canvas.h"
#include <algorithm>
#include <iostream>

Canvas::Canvas(int width, int height) : width(width), height(height)
{
    // 1. Create a CPU image buffer (Format: RGBA 8-bit per channel)
    canvas = GenImageColor(width, height, BLACK);

    // 2. Load an empty texture to the GPU that we will update every frame
    targetTexture = LoadTextureFromImage(canvas);
}

Canvas::~Canvas()
{
    std::cout << "Destroying Canvas" << std::endl;
    UnloadTexture(targetTexture);
    UnloadImage(canvas);
}

void Canvas::Clear()
{
    ImageClearBackground(&canvas, clearColor);
}

void Canvas::SetClearColor(Color c)
{
    clearColor = c;
}

void Canvas::PutPixel(int x, int y, Color c)
{
    if (x >= 0 && x < width && y >= 0 && y < height)
    {
        ((Color *)canvas.data)[y * width + x] = c;
    }
}

void Canvas::Update()
{
    // Update the existing GPU texture with new CPU pixel data
    UpdateTexture(targetTexture, canvas.data);
}

void Canvas::Blit(int x, int y)
{
    // Draw your software buffer to the screen
    DrawTexture(targetTexture, x, y, WHITE);
}