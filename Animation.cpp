#include "Animation.h"
#include <iostream> // Añadido para std::cerr

Animation::Animation(unsigned short frame_width, const std::string& texture_location, unsigned short animation_speed)
    : frame_width(frame_width), animation_speed(animation_speed), flipped(false),
    animation_iterator(0), current_frame(0) {

    if (!texture.loadFromFile(texture_location)) {
        // Manejo de error mejorado
        std::cerr << "Error: No se pudo cargar la textura desde " << texture_location << std::endl;
        
    }

    sprite.setTexture(texture);
    // Asegurarse de que frame_width no sea cero para evitar división por cero
    if (frame_width > 0) {
        total_frames = texture.getSize().x / frame_width;
    }
    else {
        total_frames = 0;
        std::cerr << "Error: frame_width es 0 en Animation constructor para " << texture_location << std::endl;
    }


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
    // Adjust origin for flipping if sprite is not centered
    // For simplicity, assuming origin is top-left (0,0) for setScale
    // If you flip around the center, you'd use:
    // sprite.setOrigin(frame_width / 2.0f, texture.getSize().y / 2.0f);
    if (flipped) {
        sprite.setScale(-1.0f, 1.0f);
        // If flipping affects position (e.g. origin is 0,0), you might need to adjust position:
        // sprite.setPosition(sprite.getPosition().x + frame_width, sprite.getPosition().y);
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
        std::cerr << "Error: No se pudo cargar la textura desde " << location << " en set_texture_location" << std::endl;
        // Handle error, maybe set total_frames to 0 or keep old texture?
        total_frames = 0; // Example: invalidate animation
        return;
    }
    sprite.setTexture(texture); // Esto es importante, re-aplicar la textura al sprite
    if (frame_width > 0) {
        total_frames = texture.getSize().x / frame_width;
    }
    else {
        total_frames = 0;
        std::cerr << "Error: frame_width es 0 en set_texture_location para " << location << std::endl;
    }
    // Reset current_frame and animation_iterator to avoid issues with new texture sheet
    current_frame = 0;
    animation_iterator = 0;
    sprite.setTextureRect(sf::IntRect(0, 0, frame_width, texture.getSize().y));
}

void Animation::update() {
    if (total_frames == 0 || animation_speed == 0) return; // No actualizar si no hay frames o la velocidad es 0

    animation_iterator++;
    if (animation_iterator >= animation_speed) {
        animation_iterator = 0;
        current_frame = (current_frame + 1) % total_frames;

        int frame_x = current_frame * frame_width;
        sprite.setTextureRect(sf::IntRect(frame_x, 0, frame_width, texture.getSize().y));
    }
}
