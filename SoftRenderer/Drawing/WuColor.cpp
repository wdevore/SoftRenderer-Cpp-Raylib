#include <iostream>
#include <cmath>

#include "WuColor.h"

WuColor::WuColor(/* args */)
{
}

WuColor::~WuColor()
{
}

void WuColor::initialize(CColor bg, CColor fg, int intensityBits)
{
    color = fg;
    setIntensityBits(intensityBits);

    wuIntensities.resize(numIntensityLevels);

    // Color ratio = (background - linecolor)/NumLevels
    float crR = (float)(bg.r - fg.r) / (float)numIntensityLevels;
    float crG = (float)(bg.g - fg.g) / (float)numIntensityLevels;
    float crB = (float)(bg.b - fg.b) / (float)numIntensityLevels;

    for (float i = 1.0f; i < (float)numIntensityLevels + 1; i += 1.0f)
    {
        CColor &cu = wuIntensities[(int)i - 1];
        cu.r = (int)((float)fg.r + (crR * i));
        cu.g = (int)((float)fg.g + (crG * i));
        cu.b = (int)((float)fg.b + (crB * i));
        cu.a = 255;
        // std::cout << "i= " << i << " : " << cu << std::endl;
    }
}

void WuColor::setIntensityBits(int numberOfBits)
{
    intensityBits = numberOfBits;
    numIntensityLevels = 1 << intensityBits; // std::pow(2, intensityBits);
}

CColor &WuColor::GetColor(int weighting)
{
    return wuIntensities[weighting];
}
