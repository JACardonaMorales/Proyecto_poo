#include "Global.h"
#include "Life.h"
#include "MapManager.h"

Life::Life(float x, float y)
    : x(x), y(y), start_y(static_cast<unsigned short>(y)), dead(false),
    horizontal_direction(1), vertical_speed(0.0f) {

    if (!texture.loadFromFile("textures/life.png")) {
        // Handle error
    }
    sprite.setTexture(texture);
}

bool Life::get_used() const {
    return dead;
}

void Life::draw(float view_x, sf::RenderWindow& window) {
    if (!dead) {
        sprite.setPosition(x - view_x, y);
        window.draw(sprite);
    }
}

void Life::set_used(bool value) {
    dead = value;
}

void Life::update(float view_x, MapManager& map_manager) {
    if (dead) return;

    // Horizontal movement
    x += horizontal_direction * 1.0f;

    // Vertical movement with gravity
    vertical_speed += 0.2f;
    y += vertical_speed;

    // Check for collisions
    std::vector<Cell> solid_cells = { Cell::Wall, Cell::Grass, Cell::Prock };
    auto collisions = map_manager.map_collision(solid_cells, get_hit_box());

    if (!collisions.empty()) {
        // Hit ground, bounce or reverse direction
        if (vertical_speed > 0) {
            vertical_speed = -2.0f;
        }
        horizontal_direction *= -1;
    }

    // Remove if too far from start position or off screen
    if (y > start_y + 100 || x < view_x - 50 || x > view_x + 320) {
        dead = true;
    }
}

sf::FloatRect Life::get_hit_box() const {
    return sf::FloatRect(x, y, 16, 16);
}