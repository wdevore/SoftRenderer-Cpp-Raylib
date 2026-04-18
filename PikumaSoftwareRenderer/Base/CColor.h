#pragma once

#include <string>
#include <ostream>

// CColor can also be the foreground color of a WuColor.
struct CColor
{
    int r;
    int g;
    int b;
    int a;

    static CColor BgDarkGray;
    static CColor Red;
    static CColor White;
    static CColor Black;
    static CColor Yellow;
    static CColor Green;
    static CColor Blue;
    static CColor Magenta;
    static CColor Cyan;

    static CColor LightGreen;
    static CColor Orange;

    CColor() : r(128), g(128), b(128), a(255) {}
    CColor(int r, int g, int b, int a = 255) : r(r), g(g), b(b), a(a) {}

    void Set(CColor color);
    void Set(int r, int g, int b, int a = 255);

    friend std::ostream &operator<<(std::ostream &os, const CColor &c)
    {
        return os << std::to_string(c.r) + ", " + std::to_string(c.g) + ", " + std::to_string(c.b) + ", " + std::to_string(c.a);
    }
};

inline CColor CColor::BgDarkGray{80, 80, 80, 255};
inline CColor CColor::Red{255, 0, 0, 255};
inline CColor CColor::White{255, 255, 255, 255};
inline CColor CColor::Black{0, 0, 0, 255};
inline CColor CColor::Yellow{255, 255, 0, 255};
inline CColor CColor::Green{0, 255, 0, 255};
inline CColor CColor::Blue{0, 0, 255, 255};
inline CColor CColor::Magenta{255, 0, 255, 255};
inline CColor CColor::Cyan{0, 255, 255, 255};

inline CColor CColor::LightGreen{64, 255, 64, 255};
inline CColor CColor::Orange{255, 128, 0, 255};
