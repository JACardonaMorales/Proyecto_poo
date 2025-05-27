#pragma once

#include <SFML/Graphics.hpp>

class Animation {
private:
    bool flipped;
    unsigned short animation_iterator;
    unsigned short animation_speed;
    unsigned short current_frame;
    unsigned short frame_width;
    unsigned short total_frames;
    sf::Sprite sprite;
    sf::Texture texture;

public:
    Animation(unsigned short frame_width, const std::string& texture_location, unsigned short animation_speed);
    void draw(sf::RenderWindow& window);
    void set_animation_speed(unsigned short speed);
    void set_flipped(bool value);
    void set_position(float x, float y);
    void set_texture_location(const std::string& location);
    void update();
    sf::Sprite& get_sprite() { return sprite; }
};

