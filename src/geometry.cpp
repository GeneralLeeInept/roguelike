#include "geometry.h"

#include <algorithm>

Point::Point(int x, int y)
    : x(x)
    , y(y)
{
}

Point operator+(const Point& a, const Point& b)
{
    return Point(a.x + b.x, a.y + b.y);
}

Point operator-(const Point& a, const Point& b)
{
    return Point(a.x - b.x, a.y - b.y);
}

Point operator*(const Point& a, int scale)
{
    return Point(a.x * scale, a.y * scale);
}

Point operator/(const Point& a, int denom)
{
    return Point(a.x / denom, a.y / denom);
}

bool operator!=(const Point& a, const Point& b)
{
    return a.x != b.x || a.y != b.y;
}

Rectangle::Rectangle(const Point& mins, const Point& maxs)
    : mins(mins)
    , maxs(maxs)
{
}

Rectangle::Rectangle(const Point& centre, int width, int height)
{
    Point size(width, height);
    mins = centre - size / 2;
    maxs = mins + size;
}

Point Rectangle::centre() const
{
    return (mins + maxs) / 2;
}

// Return true if r1 & r2 are separated by at least min_separation in both x & y
bool Rectangle::separated(const Rectangle& r1, const Rectangle& r2, int min_separation)
{
    Point sep_a = r1.mins - r2.maxs;
    Point sep_b = r2.mins - r1.maxs;
    return (sep_a.x >= min_separation || sep_a.y >= min_separation || sep_b.x >= min_separation || sep_b.y >= min_separation);
}

Rectangle Rectangle::intersection(const Rectangle& r1, const Rectangle& r2)
{
    Rectangle r;
    r.mins.x = std::max(r1.mins.x, r2.mins.x);
    r.mins.y = std::max(r1.mins.y, r2.mins.y);
    r.maxs.x = std::min(r1.maxs.x, r2.maxs.x);
    r.maxs.y = std::min(r1.maxs.y, r2.maxs.y);
    return r;
}
