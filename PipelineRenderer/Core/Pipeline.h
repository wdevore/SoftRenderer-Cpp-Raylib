#pragma once

#include "Canvas.h"

class Pipeline
{
private:
    /* data */
    int width{};
    int height{};

    Canvas canvas{};

public:
    Pipeline(int width, int height) : width(width), height(height) {};
    ~Pipeline();

    void Setup();
    void Begin();
    void Update();
    void End();
};
