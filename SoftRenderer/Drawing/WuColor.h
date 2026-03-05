#pragma once

#include <vector>

#include "CColor.h"

class WuColor
{
private:
    /* data */
public:
    std::vector<PaintColoring::CColor> wuIntensities{};

    PaintColoring::CColor color{};
    int intensityBits{};
    int numIntensityLevels{};

    WuColor(/* args */);
    ~WuColor();

    /// @brief
    /// @param bg Sets the background blending color.
    /// @param fg Sets object's color property
    /// @param intensityBits
    void initialize(PaintColoring::CColor bg, PaintColoring::CColor fg, int intensityBits);
    void setIntensityBits(int numberOfBits);
    PaintColoring::CColor &GetColor(int weighting);
};
