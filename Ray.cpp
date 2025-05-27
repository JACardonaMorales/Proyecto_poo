#include "stdafx.h"
#include "Ray.h"
#include "MapManager.h"
#include <SFML/Window.hpp>

Ray::Ray()
    : walk_animation(32, "assets/Ray/textures/RayMovement.png", 3),
    lifes(3), dead(false), flipped(false), on_ground(false),
    enemy_bounce_speed(0.0f), horizontal_speed(0.0f), vertical_speed(0.0f),
    x(100.0f), y(100.0f), jump_timer(0), powerup_state(0),
    death_timer(0), life_take_timer(0), invincible_timer(0) {

    if (!texture.loadFromFile("assets/Ray/textures/RayMovement.png")) {
        // Handle error
    }
    sprite.setTexture(texture);
}

bool Ray::get_dead() const {
    return dead;
}

float Ray::get_vertical_speed() const {
    return vertical_speed;
}

float Ray::get_x() const {
    return x;
}

int Ray::get_lifes() const {
    return lifes;
}

void Ray::set_lifes(int value) {
    lifes = value;
}

void Ray::die(bool instant_death) {
    if (instant_death) {
        dead = true;
        death_timer = 0;
    }
    else {
        lifes--;
        if (lifes <= 0) {
            dead = true;
        }
        invincible_timer = 120; // 2 seconds of invincibility
    }
}

void Ray::draw(sf::RenderWindow& window) {
    if (invincible_timer > 0 && (invincible_timer / 4) % 2 == 0) {
        // Flicker during invincibility
        return;
    }

    if (horizontal_speed != 0) {
        walk_animation.set_position(x, y);
        walk_animation.set_flipped(flipped);
        walk_animation.draw(window);
    }
    else {
        sprite.setPosition(x, y);
        if (flipped) {
            sprite.setScale(-1.0f, 1.0f);
        }
        else {
            sprite.setScale(1.0f, 1.0f);
        }
        window.draw(sprite);
    }
}

void Ray::draw_leafs(float view_x, sf::RenderWindow& window) {
    for (auto& life : life_objects) {
        life.draw(view_x, window);
    }
}

void Ray::reset() {
    dead = false;
    lifes = 3;
    x = 100.0f;
    y = 100.0f;
    horizontal_speed = 0.0f;
    vertical_speed = 0.0f;
    flipped = false;
    on_ground = false;
    jump_timer = 0;
    death_timer = 0;
    invincible_timer = 0;
    life_take_timer = 0;
    powerup_state = 0;
    enemy_bounce_speed = 0.0f;
    life_objects.clear();
}

void Ray::set_position(float x, float y) {
    this->x = x;
    this->y = y;
}

void Ray::set_vertical_speed(float value) {
    vertical_speed = value;
}

void Ray::update(float view_x, MapManager& map_manager) {
    if (dead) {
        death_timer++;
        return;
    }

    // Decrease timers
    if (invincible_timer > 0) invincible_timer--;
    if (life_take_timer > 0) life_take_timer--;
    if (jump_timer > 0) jump_timer--;

    // Input handling
    horizontal_speed = 0.0f;

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
        horizontal_speed = -2.0f;
        flipped = true;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
        horizontal_speed = 2.0f;
        flipped = false;
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space) && on_ground && jump_timer == 0) {
        vertical_speed = -5.0f;
        on_ground = false;
        jump_timer = 10;
    }

    // Apply gravity
    vertical_speed += 0.2f;
    if (vertical_speed > 6.0f) {
        vertical_speed = 6.0f;
    }

    // Movement
    x += horizontal_speed;
    y += vertical_speed;

    // Collision detection
    std::vector<Cell> solid_cells = { Cell::Wall, Cell::Grass, Cell::Prock };
    auto collisions = map_manager.map_collision(solid_cells, get_hit_box());

    if (!collisions.empty()) {
        if (vertical_speed > 0) {
            // Landing on ground
            on_ground = true;
            vertical_speed = 0.0f;
            // Snap to grid
            y = static_cast<int>(y / 16) * 16;
        }
        else if (vertical_speed < 0) {
            // Hit ceiling
            vertical_speed = 0.0f;
        }
    }
    else {
        on_ground = false;
    }

    // Update animations
    if (horizontal_speed != 0) {
        walk_animation.update();
    }

    // Update life objects
    for (auto& life : life_objects) {
        life.update(view_x, map_manager);
    }

    // Remove used life objects
    life_objects.erase(
        std::remove_if(life_objects.begin(), life_objects.end(),
            [](const Life& life) { return life.get_used(); }),
        life_objects.end());

    // Death by falling
    if (y > 240) {
        die(true);
    }
}

sf::FloatRect Ray::get_hit_box() const {
    return sf::FloatRect(x, y, 16, 16);
}