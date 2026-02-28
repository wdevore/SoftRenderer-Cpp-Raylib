#pragma once

#include <algorithm>

namespace Maths
{
    class Rectangle
    {
    public:
        int x;
        int y;
        int width;
        int height;

        Rectangle();
        Rectangle(int width, int height);
        Rectangle(int x, int y, int width, int height);
        Rectangle(const Rectangle &r);

        double getX() const;
        double getY() const;
        double getWidth() const;
        double getHeight() const;

        void setRect(int x, int y, int width, int height);
        void setBounds(int x, int y, int width, int height);

        bool contains(int X, int Y) const;
        bool intersects(const Rectangle &r) const;
        Rectangle intersection(const Rectangle &r) const;
        Rectangle Union(const Rectangle &r) const;
        bool isEmpty() const;
    };
} // namespace Maths
