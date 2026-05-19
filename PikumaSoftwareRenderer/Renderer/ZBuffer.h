#pragma once

#include <vector>

class ZBuffer
{
public:
    ZBuffer();
    ~ZBuffer();

    void Initialize(int width, int height);
    void reset();
    int getIndex(int col, int row);
    int setZ(int col, int row, float zv, bool debug);
    int setZ(int i, float zv, bool debug);
    void setZ(int col, int row, float zv);
    float getZ(int col, int row);

private:
    int width{};
    int height{};
    int zsize{};

    std::vector<float> z{};
};