#pragma once

#include <vector>

#include "CColor.h"

class WuColor
{
private:
    /* data */
public:
    std::vector<CColor> wuIntensities{};

    CColor color{};
    int intensityBits{};
    int numIntensityLevels{};

    WuColor(/* args */);
    ~WuColor();

    void initialize(CColor bg, CColor fg, int intensityBits);
    void setIntensityBits(int numberOfBits);
    CColor &GetColor(int weighting);
};
