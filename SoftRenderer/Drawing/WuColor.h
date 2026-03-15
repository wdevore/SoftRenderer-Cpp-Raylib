#pragma once

#include <vector>

#include "CColor.h"

namespace PaintColoring
{
    class WuColor
    {
    private:
        /* data */
    public:
        std::vector<PaintColoring::CColor> wuIntensities{};
        inline static int Bit8IntensityLevels = 8;
        inline static int Bit16IntensityLevels = 16;

        PaintColoring::CColor color{};
        int intensityBits{};
        int numIntensityLevels{};

        static WuColor BgDarkGray;
        static WuColor Red;
        static WuColor White;
        static WuColor Black;
        static WuColor Yellow;
        static WuColor Green;
        static WuColor Blue;
        static WuColor Magenta;
        static WuColor Cyan;

        WuColor(/* args */);
        WuColor(PaintColoring::CColor bg, PaintColoring::CColor fg, int intensityBits);
        ~WuColor();

        /// @brief
        /// @param bg Sets the background blending color.
        /// @param fg Sets object's color property
        /// @param intensityBits
        void initialize(PaintColoring::CColor bg, PaintColoring::CColor fg, int intensityBits);

        void setIntensityBits(int numberOfBits);

        PaintColoring::CColor &GetColor(int weighting);

        // ============ Helper methods ===================
        void SetFromCColor(PaintColoring::CColor c)
        {
            initialize(CColor::White, c, 8);
        }
    };

    inline WuColor WuColor::BgDarkGray{CColor::White, CColor::BgDarkGray, 8};
    inline WuColor WuColor::Red{CColor::White, CColor::Red, 8};
    inline WuColor WuColor::White{CColor::White, CColor::White, 8};
    inline WuColor WuColor::Black{CColor::White, CColor::Black, 8};
    inline WuColor WuColor::Yellow{CColor::White, CColor::Yellow, 8};
    inline WuColor WuColor::Green{CColor::White, CColor::Green, 8};
    inline WuColor WuColor::Blue{CColor::White, CColor::Blue, 8};
    inline WuColor WuColor::Magenta{CColor::White, CColor::Magenta, 8};
    inline WuColor WuColor::Cyan{CColor::White, CColor::Cyan, 8};

}
