#include <algorithm>
#include <iostream>
#include <cmath>

#include "Canvas.h"

Canvas::Canvas()
{
}

Canvas::~Canvas()
{
    std::cout << "Destroying Canvas" << std::endl;
    UnloadTexture(targetTexture);
    UnloadImage(canvas);
}

void Canvas::initialize(int width, int height)
{
    this->width = width;
    this->height = height;

    // 1. Create a CPU image buffer (Format: RGBA 8-bit per channel)
    canvas = GenImageColor(width, height, BLACK);

    // 2. Load an empty texture to the GPU that we will update every frame
    targetTexture = LoadTextureFromImage(canvas);
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

Color Canvas::GetPixel(int x, int y)
{
    if (x < 0 || x > width - 1 || y < 0 || y > height - 1)
    {
        return BLACK;
    }

    return ((Color *)canvas.data)[y * width + x];
}

/// @brief Update the existing GPU texture with new CPU pixel data
void Canvas::Update()
{
    UpdateTexture(targetTexture, canvas.data);

    // Another object should reset the Z Buffer after updating texture.
}

/// @brief Draw software buffer to the screen
void Canvas::Blit(int x, int y)
{
    DrawTexture(targetTexture, x, y, WHITE);
}
