#pragma once

#include <string>
#include <ostream>

namespace PaintColoring
{
    struct CColor
    {
        int r;
        int g;
        int b;
        int a;

        static const CColor BgDarkGray;
        static const CColor Red;
        static const CColor White;
        static const CColor Black;
        static const CColor Yellow;
        static const CColor Green;
        static const CColor Blue;
        static const CColor Magenta;
        static const CColor Cyan;

        CColor() : r(128), g(128), b(128), a(255) {}
        CColor(int r, int g, int b, int a = 255) : r(r), g(g), b(b), a(a) {}

        friend std::ostream &operator<<(std::ostream &os, const CColor &c)
        {
            return os << std::to_string(c.r) + ", " + std::to_string(c.g) + ", " + std::to_string(c.b) + ", " + std::to_string(c.a);
        }
    };

    inline const CColor CColor::BgDarkGray{80, 80, 80, 255};
    inline const CColor CColor::Red{255, 0, 0, 255};
    inline const CColor CColor::White{255, 255, 255, 255};
    inline const CColor CColor::Black{0, 0, 0, 255};
    inline const CColor CColor::Yellow{255, 255, 0, 255};
    inline const CColor CColor::Green{0, 255, 0, 255};
    inline const CColor CColor::Blue{0, 0, 255, 255};
    inline const CColor CColor::Magenta{255, 0, 255, 255};
    inline const CColor CColor::Cyan{0, 255, 255, 255};

} // namespace Drawing
