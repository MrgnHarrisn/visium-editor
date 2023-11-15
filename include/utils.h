
#include <SFML/Graphics.hpp>
#include <cmath>
#include <limits>
#include <string>

#include <iostream>
#include <fstream>

#include <algorithm>

#include "sector.h"

#define VERSION "v1_4"

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

struct CameraInfo
{
    sf::Vector2f pos = {0, 0};
    uint16_t sec_id = 0;
};

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

/// @brief Get's the closest point on a line to a position in space
/// @param seg line to check against
/// @param P point we want to find closest
/// @return vector along the line
sf::Vector2f p2seg(Segment &seg, sf::Vector2f P)
{
    sf::Vector2f A = point_v2f(seg.start);
    sf::Vector2f B = point_v2f(seg.end);
    sf::Vector2f AB = B - A;
    sf::Vector2f AP = P - A;

    /*  */
    float scalar_projection = (AP.x * AB.x + AP.y * AB.y) / (AB.x * AB.x + AB.y * AB.y);

    sf::Vector2f C = A + scalar_projection * AB;
    return C;
}

/// @brief Get's the distance betwe
/// @tparam T1
/// @tparam T2
/// @param seg line to check against
/// @param p
/// @param epsilon
/// @return
bool is_close(Segment &seg, sf::Vector2f p, float epsilon)
{
    sf::Vector2f C = p2seg(seg, p);
    /* Check if distance is less than the required to count */
    return p_dist(C, p) < epsilon;
}

/// @brief Sets a given Point to a point of one or the other
/// @param s_s distance from start to start (of points)
/// @param s_e distance from start to end (of points)
/// @param result the Point we wish to set
/// @param seg The given segment we need to find which to set
/// @param epsilon smallest distance it should snap to
void set_nearest_point(float dist1, float dist2, Point &result, Segment &seg, float epsilon)
{
    if (dist1 < epsilon || dist2 < epsilon)
    {
        Point &selectedPoint = (dist1 < dist2) ? seg.start : seg.end;
        if (std::isnan(result.x))
        {
            result = selectedPoint;
        }
        else
        {
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
    if (segments.size() == 0)
    {
        return result;
    }

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

/// @brief Gets closest start and end
/// @param seg the line we are checking for
/// @param segments the set of segments we compare points to
/// @param epsilon the distance required to be a valid distance
/// @return Either returns a segment with a value or NaN for x and y
Segment closest_point_portal(Segment *s, std::vector<Segment> segments, Segment* portal, float epsilon)
{
    Segment result;

    /* If there is nothing to check against then don't */
    if (segments.size() == 0)
    {
        return result;
    }

    for (Segment seg : segments)
    {
        float s_s = p_dist(s->start, seg.start);
        float s_e = p_dist(s->start, seg.end);
        float e_s = p_dist(s->end, seg.start);
        float e_e = p_dist(s->end, seg.end);

        set_nearest_point(s_s, s_e, result.start, seg, epsilon);
        set_nearest_point(e_s, e_e, result.end, seg, epsilon);
    }

    float s_s = p_dist(s->start, portal->start);
    float s_e = p_dist(s->start, portal->end);
    float e_s = p_dist(s->end, portal->start);
    float e_e = p_dist(s->end, portal->end);

    set_nearest_point(s_s, s_e, result.start, *s, epsilon);
    set_nearest_point(e_s, e_e, result.end, *s, epsilon);

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

/// @brief Checks if a point is inside an area
/// @param position top left of area
/// @param size size of the area
/// @param pos position of the point
/// @return
bool box_contains_pos(sf::Vector2f position, sf::Vector2f size, sf::Vector2f pos)
{
    return pos.x >= position.x && pos.y > position.y && pos.x <= position.x + size.x && pos.y <= position.y + size.y;
}

int vec_contains_color(std::vector<sf::Color> vec, sf::Color col)
{
    for (int i = 0; i < vec.size(); i++)
    {
        if (vec[i] == col)
        {
            return i;
        }
    }
    return -1;
}

std::vector<sf::Color> unique_colors(std::vector<Sector *> sectors)
{
    std::vector<sf::Color> colors;
    for (Sector *sec : sectors)
    {
        for (int i = 0; i < sec->segs.size(); i++)
        {
            int col_index = vec_contains_color(colors, sec->segs[i].color);
            if (col_index == -1)
            {
                colors.push_back(sec->segs[i].color);
                sec->segs[i].color_id = colors.size() - 1;
            }
            else
            {
                sec->segs[i].color_id = col_index;
            }
        }
    }

    return colors;
}

std::string color_to_str(sf::Color color)
{
    std::string output = "";
    output += std::to_string(color.r) + " " + std::to_string(color.g) + " " + std::to_string(color.b) + "\n";
    return output;
}

bool contains_same_segment(const Segment &seg, Sector *sec)
{
    // Check if the segment's start and end points match any existing segment in the sector
    auto it = std::find_if(sec->segs.begin(), sec->segs.end(), [&](const Segment &s)
                           { return (s.start.x == seg.start.x && s.start.y == seg.start.y &&
                                     s.end.x == seg.end.x && s.end.y == seg.end.y) ||
                                    (s.start.x == seg.end.x && s.start.y == seg.end.y &&
                                     s.end.x == seg.start.x && s.end.y == seg.start.y); });

    return it != sec->segs.end(); // Returns true if a matching segment is found
}

void add_other_portal(Segment seg, int16_t parent_id, std::vector<Sector *> sectors)
{

    for (Sector *sec : sectors)
    {
        if (sec->id == seg.portal_id)
        {
            seg.portal_id = parent_id;
            if (!contains_same_segment(seg, sec))
            {
                sec->segs.push_back(seg);
            }
        }
    }
}

void serialize(std::vector<Sector *> sectors, std::vector<sf::Color> colors, CameraInfo &inf)
{
    /* File name can be 256 chars long */
    std::string filename = "";
    printf("Enter filename: ");
    std::cin >> filename;
    filename += ".wad";
    std::fstream output_file;
    output_file.open(filename, std::ios::out);

    /* Add parallel portals for other sector */
    for (Sector *sec : sectors)
    {
        for (Segment seg : sec->segs)
        {
            if (seg.portal_id != -1)
            {
                add_other_portal(seg, sec->id, sectors);
            }
        }
    }

    /* Itterate over sectors write them into file */
    // V not_really_a_version
    unsigned int size = 0;
    for (Sector *sec : sectors)
    {
        if (sec->segs.size() > 0)
        {
            size++;
        }
    }

    output_file << VERSION << "\n"; //" SECS " << size << "\n\n";
    output_file << "PPOS\n"
                << inf.pos.x << " " << inf.pos.y << " " << inf.sec_id << "\n";
    output_file << "COLS " << colors.size() << "\n";

    /* Add Colors in order of usage [index is ID] */
    for (sf::Color col : colors)
    {
        output_file << color_to_str(col);
    }

    output_file << "SECS " << size << "\n";

    for (Sector *sec : sectors)
    {
        /* Sector ID */
        if (sec->segs.size() > 0)
        {
            output_file << "ID " << sec->id << " NUM_W " << sec->segs.size() << "\n";

            for (Segment seg : sec->segs)
            { /* No sex inuendos here */
                output_file << seg.start.x << " " << seg.start.y << " " << seg.end.x << " " << seg.end.y << " " << seg.color_id << " "
                            << seg.portal_id << "\n";
            }
            output_file << "\n";
        }
    }

    output_file.close();
    printf("Saved!\n");
}