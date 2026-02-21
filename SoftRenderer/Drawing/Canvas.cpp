#include "Canvas.h"
#include <algorithm>

Canvas::Canvas(int width, int height) : width(width), height(height)
{
    pixels.resize(width * height, BLACK);

    // 1. Create a CPU image buffer (Format: RGBA 8-bit per channel)
    Image img = GenImageColor(width, height, BLACK);

    // 2. Load an empty texture to the GPU that we will update every frame
    texture = LoadTextureFromImage(img);

    UnloadImage(img);
}

Canvas::~Canvas()
{
    UnloadTexture(texture);
}

void Canvas::Clear()
{
    std::fill(pixels.begin(), pixels.end(), clearColor);
}

void Canvas::SetClearColor(Color c)
{
    clearColor = c;
}

void Canvas::PutPixel(int x, int y, Color c)
{
    if (x >= 0 && x < width && y >= 0 && y < height)
    {
        pixels[y * width + x] = c;
    }
}

void Canvas::Update()
{
    // Update the existing GPU texture with new CPU pixel data
    UpdateTexture(texture, pixels.data());
}

void Canvas::Blit(int x, int y)
{
    DrawTexture(texture, x, y, WHITE);
}