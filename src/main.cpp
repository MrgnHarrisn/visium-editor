#include <SFML/Graphics.hpp>
#include <vector>

#include "utils.h"  // this includes point so yeah

/* Mmmmmmm */
/* Italian Code */

int main() {
    sf::RenderWindow window(sf::VideoMode(800, 800), "Acta Editor", sf::Style::None | sf::Style::Close);

    Point offset;
    offset.x = 0;
    offset.y = 0;

    std::vector<int> segs_made;

    std::vector<Sector*> sectors;
    
    int sec_id = 0;

    Sector* current_sec = new Sector;
    current_sec->id = sec_id;
    sectors.push_back(current_sec);

    /* Start and end point for a segment */
    Point start, end;

    sf::Vector2i smp = sf::Mouse::getPosition();
    start.x = smp.x;
    start.y = smp.y;

    bool holding = false;
    bool panning = false;
    sf::Vector2i lastPanPosition; // Add this line to store the last pan position


    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            } else if (event.type == sf::Event::MouseButtonPressed) {
                if (event.mouseButton.button == sf::Mouse::Left) {
                    /* Left button is pressed */
                    /* Get mouse position and wait for release */
                    if (!holding) {     // if not holding already
                        holding = true;
                        start = v2i_point(sf::Mouse::getPosition(window));
                        // printf("Pressed\n");
                    }
                } else if (event.mouseButton.button == sf::Mouse::Right) {  // are we trying to pan?
                    if (!panning) {
                        panning = true;
                        window.setMouseCursorVisible(false);
                        lastPanPosition = sf::Mouse::getPosition(window);
                    }
                }
            } else if (event.type == sf::Event::MouseButtonReleased) {  // Mouse buttons released
                if (event.mouseButton.button == sf::Mouse::Left) {  // check if it was the left button
                    if (holding) {
                        // printf("Releasing\n");
                        holding = false;
                        end = v2i_point(sf::Mouse::getPosition(window));

                        /* Create segment */
                        Segment* seg = new Segment;
                        seg->start = Point::sub(start, offset);
                        seg->end = Point::sub(end, offset);

                        /* Snap to segment in sector */
                        Segment temp_s = closest_point(seg, current_sec->segs, 20);  // I think this is in pixels?

                        /* Figure out which numbers to set */
                        if (!std::isnan(temp_s.start.x)) {
                            seg->start.x = temp_s.start.x;
                        }
                        if (!std::isnan(temp_s.start.y)) {
                            seg->start.y = temp_s.start.y;
                        }
                        if (!std::isnan(temp_s.end.x)) {
                            seg->end.x = temp_s.end.x;
                        }
                        if (!std::isnan(temp_s.end.y)) {
                            seg->end.y = temp_s.end.y;
                        }

                        /* Add segment to vector of walls */
                        current_sec->segs.push_back(*seg);

                        /* for undoing stuff fuck you */
                        segs_made.push_back(sec_id);

                        /* Delete the segment */
                        delete(seg);
                    }
                } else if (event.mouseButton.button == sf::Mouse::Right) {
                    // we are no longer trying to pan
                    panning = false;
                    window.setMouseCursorVisible(true);
                }
            } else if (event.type == sf::Event::MouseWheelScrolled) {
                if (event.mouseWheelScroll.delta < 0) {
                    sec_id--;
                    sec_id = (sec_id < 0) ? 0 : sec_id;
                    
                } else {
                    sec_id++;
                    /* Check if this sector exists */
                    if (sec_id > sectors.size() - 1) {
                        if (sectors[sec_id-1]->segs.size() > 0) {
                            /* make the sector if not */
                            current_sec = new Sector;
                            current_sec->id = sec_id;
                            sectors.push_back(current_sec);
                        } else {
                            sec_id--;
                        }
                        
                    }
                }
                // printf("Sector Id: %d\n", sec_id);
                current_sec = sectors[sec_id];
            } else if (event.type == sf::Event::KeyPressed) {
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::S) && sf::Keyboard::isKeyPressed(sf::Keyboard::LControl)) {
                    /* Save */
                    printf("Saving\n");
                    serialize(sectors);
                } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Z) && sf::Keyboard::isKeyPressed(sf::Keyboard::LControl)) {
                    // printf("UNdoing my parents mistake\n");
                    /* Undo */
                    /* Get index of sector */
                    if (segs_made.size() > 0) {
                        int index = segs_made[segs_made.size() - 1];
                        segs_made.pop_back();
                        if (sectors[index]->segs.size() > 0) {
                            // free(sectors[index]->segs[sectors[index]->segs.size() - 1]);      /* Free memory ? */
                            sectors[index]->segs.pop_back();                                    /* Remove segment */
                        }
                    }
                }
            }

        }

        if (panning && sf::Mouse::isButtonPressed(sf::Mouse::Right)) {
            sf::Vector2i currentMousePosition = sf::Mouse::getPosition(window);
            Point panOffset;
            panOffset.x = currentMousePosition.x - lastPanPosition.x;
            panOffset.y = currentMousePosition.y - lastPanPosition.y;

            // Update your main offset
            offset.x += panOffset.x;
            offset.y += panOffset.y;

            // Reset the mouse to the initial pan position
            sf::Mouse::setPosition(lastPanPosition, window);
        }

        window.clear(sf::Color::Black);
        int i = 0;
        for (Sector* sec : sectors) {
            for (Segment s : sec->segs) {
                draw_line(window, s.start, s.end, offset, (i == sec_id ? sf::Color::Green : sf::Color::White));
            }
            i++;
        }
        window.display();
    }

    /* Memry management */
    for (Sector* sec : sectors) {
        delete(sec);
    }
    sectors.clear();

    return 0;
}
