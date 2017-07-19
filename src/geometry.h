#pragma once

#include <algorithm>

class Point
{
public:
    Point() = default;
    Point(int x, int y);

    int x;
    int y;
};

Point operator+(const Point& a, const Point& b);
Point operator-(const Point& a, const Point& b);
Point operator*(const Point& a, int scale);
Point operator/(const Point& a, int denom);
bool operator==(const Point& a, const Point& b);
bool operator!=(const Point& a, const Point& b);

class Rectangle
{
public:
    Rectangle() = default;
    Rectangle(const Point& mins, const Point& maxs);
    Rectangle(const Point& centre, int width, int height);

    Point centre() const;
    Point size() const;

    static bool separated(const Rectangle& r1, const Rectangle& r2, int min_separation = 0);
    static Rectangle intersection(const Rectangle& r1, const Rectangle& r2);

    Point mins;
    Point maxs;
};
