#pragma once

#include <vector>
#include <cmath>

/// @brief A point in 2D space
struct Point
{
    float x, y;
    unsigned int is_portal = 0;

    Point() : x(NAN), y(NAN) {}
    Point(float xVal, float yVal) : x(xVal), y(yVal) {}

    bool isnap() {
        return std::isnan(x) || std::isnan(y);
    }
    
    static Point min(Point a, Point b)
    {
        return {std::min(a.x, b.x), std::min(a.y, b.y)};
    }

    static Point sub(Point& a, Point& b) {
        Point output;
        output.x = a.x - b.x;
        output.y = a.y - b.y;
        return output;
    }
};

/// @brief A line segment from point start to point end
struct Segment
{
    Point start, end;
    int16_t portal_id;
};

/// @brief Contains an ID and a vector<Segment>
struct Sector
{
    std::vector<Segment> segs;
    int id;
};