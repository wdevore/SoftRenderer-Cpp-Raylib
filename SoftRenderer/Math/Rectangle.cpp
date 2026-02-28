#include <limits>

#include "Rectangle.h"

namespace Maths
{
    Rectangle::Rectangle() : x(0), y(0), width(0), height(0)
    {
    }

    Rectangle::Rectangle(int width, int height) : x(0), y(0), width(width), height(height)
    {
    }

    Rectangle::Rectangle(int x, int y, int width, int height) : x(x), y(y), width(width), height(height)
    {
    }

    Rectangle::Rectangle(const Rectangle &r) : x(r.x), y(r.y), width(r.width), height(r.height)
    {
    }

    double Rectangle::getX() const
    {
        return (double)x;
    }

    double Rectangle::getY() const
    {
        return (double)y;
    }

    double Rectangle::getWidth() const
    {
        return (double)width;
    }

    double Rectangle::getHeight() const
    {
        return (double)height;
    }

    void Rectangle::setRect(int x, int y, int width, int height)
    {
        this->x = x;
        this->y = y;
        this->width = width;
        this->height = height;
    }

    void Rectangle::setBounds(int x, int y, int width, int height)
    {
        setRect(x, y, width, height);
    }

    bool Rectangle::contains(int X, int Y) const
    {
        return (X >= x && Y >= y && X < x + width && Y < y + height);
    }

    bool Rectangle::intersects(const Rectangle &r) const
    {
        int tw = width;
        int th = height;
        int rw = r.width;
        int rh = r.height;
        if (rw <= 0 || rh <= 0 || tw <= 0 || th <= 0)
        {
            return false;
        }
        int tx = x;
        int ty = y;
        int rx = r.x;
        int ry = r.y;
        rw += rx;
        rh += ry;
        tw += tx;
        th += ty;
        //      overflow || intersect
        return ((rw < rx || rw > tx) &&
                (rh < ry || rh > ty) &&
                (tw < tx || tw > rx) &&
                (th < ty || th > ry));
    }

    Rectangle Rectangle::intersection(const Rectangle &r) const
    {
        int tx1 = x;
        int ty1 = y;
        int rx1 = r.x;
        int ry1 = r.y;
        long tx2 = tx1;
        tx2 += width;
        long ty2 = ty1;
        ty2 += height;
        long rx2 = rx1;
        rx2 += r.width;
        long ry2 = ry1;
        ry2 += r.height;
        if (tx1 < rx1)
            tx1 = rx1;
        if (ty1 < ry1)
            ty1 = ry1;
        if (tx2 > rx2)
            tx2 = rx2;
        if (ty2 > ry2)
            ty2 = ry2;
        tx2 -= tx1;
        ty2 -= ty1;
        // tx2,ty2 will never overflow (they will never be
        // larger than the smallest of the two source w,h)
        // they might underflow, though...
        if (tx2 < std::numeric_limits<int>::min())
            tx2 = std::numeric_limits<int>::min();
        if (ty2 < std::numeric_limits<int>::min())
            ty2 = std::numeric_limits<int>::min();
        return Rectangle(tx1, ty1, (int)tx2, (int)ty2);
    }

    Rectangle Rectangle::Union(const Rectangle &r) const
    {
        long tx2 = width;
        long ty2 = height;
        if ((tx2 | ty2) < 0)
        {
            // This rectangle has negative dimensions...
            // If r has non-negative dimensions then it is the answer.
            // If r is non-existant (has a negative dimension), then
            // both are non-existant and we can return any non-existant
            // rectangle as a result.
            if ((r.width | r.height) < 0)
            {
                return Rectangle();
            }
            return r;
        }
        long rx2 = r.width;
        long ry2 = r.height;
        if ((rx2 | ry2) < 0)
        {
            return *this;
        }
        int tx1 = x;
        int ty1 = y;
        tx2 += tx1;
        ty2 += ty1;
        int rx1 = r.x;
        int ry1 = r.y;
        rx2 += rx1;
        ry2 += ry1;
        if (tx1 > rx1)
            tx1 = rx1;
        if (ty1 > ry1)
            ty1 = ry1;
        if (tx2 < rx2)
            tx2 = rx2;
        if (ty2 < ry2)
            ty2 = ry2;
        tx2 -= tx1;
        ty2 -= ty1;
        // tx2,ty2 will never underflow since both original rectangles
        // were non-empty
        // they might overflow, though...
        if (tx2 > std::numeric_limits<int>::min())
            tx2 = std::numeric_limits<int>::min();
        if (ty2 > std::numeric_limits<int>::min())
            ty2 = std::numeric_limits<int>::min();
        return Rectangle(tx1, ty1, (int)tx2, (int)ty2);
    }

    bool Rectangle::isEmpty() const
    {
        return (width <= 0) || (height <= 0);
    }
} // namespace Maths
