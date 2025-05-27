
#include "Animation.h"

Animation::Animation(unsigned short frame_width, const std::string& texture_location, unsigned short animation_speed)
    : frame_width(frame_width), animation_speed(animation_speed), flipped(false),
    animation_iterator(0), current_frame(0) {

    if (!texture.loadFromFile(texture_location)) {
        // Handle error
    }

    sprite.setTexture(texture);
    total_frames = texture.getSize().x / frame_width;

    // Set initial texture rectangle
    sprite.setTextureRect(sf::IntRect(0, 0, frame_width, texture.getSize().y));
}

void Animation::draw(sf::RenderWindow& window) {
    window.draw(sprite);
}

void Animation::set_animation_speed(unsigned short speed) {
    animation_speed = speed;
}

void Animation::set_flipped(bool value) {
    flipped = value;
    if (flipped) {
        sprite.setScale(-1.0f, 1.0f);
    }
    else {
        sprite.setScale(1.0f, 1.0f);
    }
}

void Animation::set_position(float x, float y) {
    sprite.setPosition(x, y);
}

void Animation::set_texture_location(const std::string& location) {
    if (!texture.loadFromFile(location)) {
        // Handle error
    }
    sprite.setTexture(texture);
    total_frames = texture.getSize().x / frame_width;
}

void Animation::update() {
    animation_iterator++;
    if (animation_iterator >= animation_speed) {
        animation_iterator = 0;
        current_frame = (current_frame + 1) % total_frames;

        int frame_x = current_frame * frame_width;
        sprite.setTextureRect(sf::IntRect(frame_x, 0, frame_width, texture.getSize().y));
    }
}