
#include <SFML/Graphics.hpp>
#include <cmath>
#include <limits>
#include <string>

#include <iostream>
#include <fstream>

#include "sector.h"

#define VERSION "v1_0"

#pragma once

/// @brief Converts a sf::Vector2i to a Point
/// @param point point to convert
/// @return Point
Point v2i_point(sf::Vector2i point)
{
    Point p;
    p.x = point.x;
    p.y = point.y;
    return p;
}

/// @brief Holds 2 points
struct p2
{
    Point x;
    Point y;
};

/// @brief converts a Point into a Vector2i
/// @param point point to convert
/// @return
sf::Vector2i point_v2i(Point point)
{
    sf::Vector2i p;
    p.x = point.x;
    p.y = point.y;
    return p;
}

/// @brief converts a Point into a Vector2f
/// @param point point to convert
/// @return
sf::Vector2f point_v2f(Point point)
{
    sf::Vector2f p;
    p.x = point.x;
    p.y = point.y;
    return p;
}

/// @brief Gets distance between two points
/// @param a point a
/// @param b point b
/// @return distance between points
template <typename T1, typename T2>
float p_dist(T1 &a, T2 b)
{
    float dist = 0.0f;
    dist = sqrt((b.x - a.x) * (b.x - a.x) + (b.y - a.y) * (b.y - a.y));
    return dist;
}

/// @brief Sets a given Point to a point of one or the other
/// @param s_s distance from start to start (of points)
/// @param s_e distance from start to end (of points)
/// @param result the Point we wish to set
/// @param seg The given segment we need to find which to set
/// @param epsilon smallest distance it should snap to
void set_nearest_point(float dist1, float dist2, Point& result, Segment& seg, float epsilon)
{
    if (dist1 < epsilon || dist2 < epsilon)
    {
        Point& selectedPoint = (dist1 < dist2) ? seg.start : seg.end;
        if (std::isnan(result.x)) {
            result = selectedPoint;
        } else {
            result = Point::min(result, selectedPoint);
        }
    }
}

/// @brief Gets closest start and end
/// @param seg the line we are checking for
/// @param segments the set of segments we compare points to
/// @param epsilon the distance required to be a valid distance
/// @return Either returns a segment with a value or NaN for x and y
Segment closest_point(Segment *s, std::vector<Segment> segments, float epsilon)
{
    Segment result;

    /* If there is nothing to check against then don't */
    if (segments.size() == 0) { return result; }

    for (Segment seg : segments)
    {
        float s_s = p_dist(s->start, seg.start);
        float s_e = p_dist(s->start, seg.end);
        float e_s = p_dist(s->end, seg.start);
        float e_e = p_dist(s->end, seg.end);

        set_nearest_point(s_s, s_e, result.start, seg, epsilon);
        set_nearest_point(e_s, e_e, result.end, seg, epsilon);
    }
    return result;
}

/// @brief Draws a vertex to the window
/// @param window window to draw to
/// @param a start point
/// @param b end point
void draw_line(sf::RenderWindow &window, Point a, Point b, Point offset, sf::Color color)
{
    sf::VertexArray line(sf::LinesStrip, 2);

    // line[0].color = line[1].color = sf::Color::White;
    line[0].color = color;
    line[1].color = color;

    line[0].position = point_v2f(a) + point_v2f(offset);
    line[1].position = point_v2f(b) + point_v2f(offset);

    window.draw(line);
}

void serialize(std::vector<Sector*> sectors)
{
    /* File name can be 256 chars long */
    std::string filename = "";
    printf("Enter filename: ");
    std::cin >> filename;
    filename += ".wad";
    std::fstream output_file;
    output_file.open(filename, std::ios::out);

    /* Itterate over sectors write them into file */
    // V not_really_a_version
    unsigned int size = 0;
    for (Sector* sec : sectors) {
        if (sec->segs.size() > 0) {
            size++;
        }
    }

    output_file << VERSION << " SECS " << size << "\n\n";

    for (Sector* sec : sectors) {
            /* Sector ID */
        if (sec->segs.size() > 0) {
            output_file << "ID " << sec->id << " NUM_W " << sec->segs.size() << "\n";

            for (Segment seg : sec->segs) {     /* No sex inuendos here */
                output_file << seg.start.x << " " << seg.start.y << " " << seg.end.x << " " << seg.end.y << "\n";
            }
            output_file << "\n";
        }
    }

    output_file.close();
    printf("Saved!\n");
}