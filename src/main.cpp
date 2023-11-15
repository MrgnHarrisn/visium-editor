#include <SFML/Graphics.hpp>
#include <vector>
#include <TGUI/TGUI.hpp>

#include "utils.h" // this includes point so yeah

/* Mmmmmmm */
/* Italian Code */

int main()
{
    sf::RenderWindow window(sf::VideoMode(1600, 800), "Visium Editor", sf::Style::None | sf::Style::Close);

    Point offset;
    offset.x = 0;
    offset.y = 0;
    bool sfmlHandleEvents = true;

    std::vector<int> segs_made;

    std::vector<Sector*> sectors;

    int sec_id = 0;

    Sector *current_sec = new Sector;
    Segment *current_seg = new Segment;
    current_seg = nullptr;
    current_sec->id = sec_id;
    sectors.push_back(current_sec);

    /* Start and end point for a segment */
    Point start, end;

    sf::Vector2i smp = sf::Mouse::getPosition();
    start.x = smp.x;
    start.y = smp.y;

    bool holding = false;
    bool panning = false;

    /* Placing Player Stuff/Camera */
    bool placing_camera = false;
    CameraInfo cam;

    /* Setting portals */
    bool setting_portal = false;
    int16_t selected_sector_portal = -1;

    sf::Vector2i lastPanPosition; // Add this line to store the last pan position

    tgui::Gui gui{window};

    auto rightPanel = tgui::ScrollablePanel::create();
    rightPanel->setPosition(1000, 0);
    rightPanel->setSize(601, 801);
    rightPanel->getRenderer()->setBackgroundColor(sf::Color{105, 105, 105, 100}); // make it gray
    rightPanel->setHorizontalScrollbarPolicy(tgui::Scrollbar::Policy::Never);

    auto layout = tgui::VerticalLayout::create();
    layout->setSize(rightPanel->getSize());

    auto seg_id_label = tgui::Label::create();
    seg_id_label->getRenderer()->setTextColor(sf::Color::White);

    auto sec_walls = tgui::Label::create();
    sec_walls->getRenderer()->setTextColor(sf::Color::White);

    auto color_picker = tgui::ColorPicker::create();
    

    // auto is_portal_option = tgui::CheckBox::create("Is Portal?");
    // is_portal_option->getRenderer()->setTextColor(sf::Color::White);

    /* Add widgets to right panel */
    layout->add(sec_walls);
    layout->add(seg_id_label);
    // layout->insertSpace(2, 0.5);
    // layout->add(is_portal_option);
    layout->add(color_picker);

    rightPanel->add(layout);
    // rightPanel->setEnabled(false);

    gui.add(rightPanel);

    sf::Color previous_color = color_picker->getColor();

    while (window.isOpen())
    {

        sf::Event event;

        while (window.pollEvent(event))
        {

            if (sfmlHandleEvents)
            {
                sf::Vector2f mousePos = static_cast<sf::Vector2f>(window.mapCoordsToPixel({event.mouseButton.x, event.mouseButton.y}));
                if (!box_contains_pos(rightPanel->getAbsolutePosition(), rightPanel->getSize(), mousePos))
                {
                    if (event.type == sf::Event::Closed)
                    {
                        window.close();
                    }
                    else if (event.type == sf::Event::MouseButtonPressed)
                    {
                        if (event.mouseButton.button == sf::Mouse::Left)
                        {
                            
                            if (placing_camera) {
                                sf::Vector2f player_pos = static_cast<sf::Vector2f>(sf::Mouse::getPosition(window));
                                cam.pos = player_pos + point_v2f(offset);
                            } else if (!holding)
                            /* Left button is pressed */
                            /* Get mouse position and wait for release */
                            { // if not holding already
                                holding = true;
                                start = v2i_point(sf::Mouse::getPosition(window));
                            }
                        }
                        else if (event.mouseButton.button == sf::Mouse::Right)
                        { // are we trying to pan?
                            if (!panning)
                            {
                                panning = true;
                                window.setMouseCursorVisible(false);
                                lastPanPosition = sf::Mouse::getPosition(window);
                            }
                        }
                    }
                    else if (event.type == sf::Event::MouseButtonReleased)
                    { // Mouse buttons released
                        if (event.mouseButton.button == sf::Mouse::Left)
                        { // check if it was the left button
                            if (holding)
                            {
                                holding = false;
                                end = v2i_point(sf::Mouse::getPosition(window));

                                /* Create segment */
                                Segment *seg = new Segment;
                                seg->start = Point::sub(start, offset);
                                seg->end = Point::sub(end, offset);

                                /* Snap to segment in sector */
                                Segment temp_s = closest_point(seg, current_sec->segs, 20); // I think this is in pixels?

                                if (p_dist(seg->start, seg->end) <= 3)
                                {
                                    continue;
                                }

                                /* Figure out which numbers to set */
                                if (!std::isnan(temp_s.start.x))
                                {
                                    seg->start.x = temp_s.start.x;
                                }

                                if (!std::isnan(temp_s.start.y))
                                {
                                    seg->start.y = temp_s.start.y;
                                }

                                if (!std::isnan(temp_s.end.x))
                                {
                                    seg->end.x = temp_s.end.x;
                                }

                                if (!std::isnan(temp_s.end.y))
                                {
                                    seg->end.y = temp_s.end.y;
                                }

                                seg->parent_id = sec_id;
                                seg->color = previous_color;

                                /* Add segment to vector of walls */
                                current_sec->segs.push_back(*seg);

                                /* for undoing stuff fuck you */
                                segs_made.push_back(sec_id);

                                /* Delete the segment */
                                delete (seg);
                            }
                        }
                        else if (event.mouseButton.button == sf::Mouse::Right)
                        {
                            // we are no longer trying to pan
                            panning = false;
                            window.setMouseCursorVisible(true);
                        }
                    }
                    else if (event.type == sf::Event::MouseWheelScrolled)
                    {
                        if (event.mouseWheelScroll.delta < 0)
                        {
                            sec_id--;
                            sec_id = (sec_id < 0) ? 0 : sec_id;
                        }
                        else
                        {
                            sec_id++;
                            /* Check if this sector exists */
                            if (sec_id > sectors.size() - 1)
                            {
                                if (sectors[sec_id - 1]->segs.size() > 0)
                                {
                                    /* make the sector if not */
                                    current_sec = new Sector;
                                    current_sec->id = sec_id;
                                    sectors.push_back(current_sec);
                                }
                                else
                                {
                                    sec_id--;
                                }
                            }
                        }
                        // printf("Sector Id: %d\n", sec_id);
                        current_sec = sectors[sec_id];
                    }
                    else if (event.type == sf::Event::KeyPressed)
                    {
                        if (sf::Keyboard::isKeyPressed(sf::Keyboard::P) && !setting_portal) {
                            setting_portal = true;
                            selected_sector_portal = sec_id;
                        } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::S) && sf::Keyboard::isKeyPressed(sf::Keyboard::LControl))
                        {
                            /* Save */
                            printf("Saving\n");
                            serialize(sectors, unique_colors(sectors), cam);
                        }
                        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Z) && sf::Keyboard::isKeyPressed(sf::Keyboard::LControl))
                        {
                            // printf("UNdoing my parents mistake\n");
                            /* Undo */
                            /* Get index of sector */
                            if (segs_made.size() > 0)
                            {
                                int index = segs_made[segs_made.size() - 1];
                                segs_made.pop_back();
                                if (sectors[index]->segs.size() > 0)
                                {
                                    // free(sectors[index]->segs[sectors[index]->segs.size() - 1]);      /* Free memory ? */
                                    sectors[index]->segs.pop_back(); /* Remove segment */
                                }
                            }
                        }
                        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q))
                        {
                            // current_seg = new Segment;
                            float closest = MAXFLOAT;
                            sf::Vector2f mouse_pos = (sf::Vector2f)sf::Mouse::getPosition(window) - point_v2f(offset);
                            int i = 0;
                            for (Segment seg : sectors[sec_id]->segs)
                            {
                                sf::Vector2f seg_p = p2seg(seg, mouse_pos);
                                float dist = p_dist(seg_p, mouse_pos);
                                if (dist < closest && std::abs(dist) < 10)
                                {
                                    current_seg = &sectors[sec_id]->segs[i];
                                    closest = dist;
                                }
                                i++;
                            }
                        } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::C)) {
                            placing_camera = !placing_camera;
                        }
                    } else if (event.type == sf::Event::KeyReleased) {
                        
                        setting_portal = false;
                        if (event.key.code == sf::Keyboard::Key::P) {
                            current_seg->portal_id = sec_id;
                            printf("Portal ID set to %d\n", current_seg->portal_id);
                        }
                    }
                }
            }
            sf::Vector2f mousePos = static_cast<sf::Vector2f>(window.mapCoordsToPixel({event.mouseButton.x, event.mouseButton.y}));
            if (!box_contains_pos({1400, 750}, {200, 50}, mousePos)) {
                gui.handleEvent(event);
            }
        }

        if (panning && sf::Mouse::isButtonPressed(sf::Mouse::Right))
        {
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
        sf::Color current_color = color_picker->getColor();
        if (previous_color != current_color) {
            previous_color = current_color;
            /* Check if the current segment selected is real */
            if (current_seg != nullptr) {
                current_seg->color = previous_color;
            }
        }

        if (placing_camera) {
            cam.sec_id = sec_id;
        }

        window.clear(sf::Color::Black);

        /* UI stuff starts here */
        seg_id_label->setText("Sector ID: " + std::to_string(sec_id));
        sec_walls->setText("Sector Walls: " + std::to_string(sectors[sec_id]->segs.size()));
        /* UI stuff ends here */

        int i = 0;
        for (Sector *sec : sectors)
        {
            for (Segment s : sec->segs)
            {
                draw_line(window, s.start, s.end, offset, (i == sec_id ? s.color : sf::Color::White));
            }
            i++;
        }

        if (current_seg != nullptr)
        {
            draw_line(window, current_seg->start, current_seg->end, offset, sf::Color::Magenta);
        }

        gui.draw();

        /* I am unironically going to kill myself */
        sf::RectangleShape fuckyou;
        fuckyou.setPosition({1400, 750});
        fuckyou.setSize({200, 50});
        // fuckyou.setFillColor(sf::Color::Magenta);
        fuckyou.setFillColor({232,228,228});

        sf::RectangleShape player_pos;
        player_pos.setPosition(cam.pos + point_v2f(offset));
        player_pos.setOrigin({5, 5});
        player_pos.setSize({10, 10});
        player_pos.setFillColor(sf::Color::Cyan);

        window.draw(player_pos);

        window.draw(fuckyou);
        window.display();
    }

    /* Memry management */
    for (Sector *sec : sectors)
    {
        delete (sec);
    }
    sectors.clear();

    return 0;
}
