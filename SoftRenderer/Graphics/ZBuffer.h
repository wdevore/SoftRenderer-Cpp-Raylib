#pragma once

#include <vector>

class ZBuffer
{
public:
    ZBuffer();
    ~ZBuffer();

    void initialize(int width, int height);
    void reset();
    int getIndex(int col, int row);
    int setZ(int col, int row, float zv, bool debug);
    int setZ(int i, float zv, bool debug);

private:
    int width{};
    int height{};
    int zsize{};

    std::vector<float> z{};
};