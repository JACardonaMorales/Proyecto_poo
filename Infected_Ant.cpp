#include "stdafx.h"
#include "Infected_Ant.h"
#include "MapManager.h"
#include "Ray.h"

Infected_Ant::Infected_Ant(bool attacks, float x, float y)
    : Enemy(x, y), walk_animation(16, "assets/Infected_Ant/enemies_walk_animation.png", 12),
    attacks(attacks), no_collision_dying(false), death_timer(0) {

    if (!texture.loadFromFile("assets/Infected_Ant/stand_animation-Sheet.png")) {
        // Handle error
    }
    sprite.setTexture(texture);

    horizontal_speed = attacks ? 1.5f : 1.0f;
}

bool Infected_Ant::get_dead(bool deletion) {
    if (deletion && death_timer > 60) {
        return true;
    }
    return dead;
}

void Infected_Ant::die(int death_type) {
    if (!dead) {
        dead = true;
        death_timer = 0;
        vertical_speed = -3.0f;
        no_collision_dying = true;
    }
}

void Infected_Ant::draw(float view_x, sf::RenderWindow& window) {
    if (dead && death_timer > 60) return;

    if (!dead && horizontal_speed != 0) {
        walk_animation.set_position(x - view_x, y);
        walk_animation.set_flipped(horizontal_speed < 0);
        walk_animation.draw(window);
    }
    else {
        sprite.setPosition(x - view_x, y);
        if (horizontal_speed < 0) {
            sprite.setScale(-1.0f, 1.0f);
        }
        else {
            sprite.setScale(1.0f, 1.0f);
        }
        window.draw(sprite);
    }
}

void Infected_Ant::update(float view_x, std::vector<std::shared_ptr<Enemy>>& enemies,
    MapManager& map_manager, Ray& ray) {
    if (dead) {
        death_timer++;
        if (no_collision_dying) {
            vertical_speed += 0.2f;
            y += vertical_speed;
        }
        return;
    }

    // Movement logic
    x += horizontal_speed;

    // Apply gravity
    vertical_speed += 0.2f;
    y += vertical_speed;

    // Check collisions with map
    std::vector<Cell> solid_cells = { Cell::Wall, Cell::Grass, Cell::Prock };
    auto collisions = map_manager.map_collision(solid_cells, get_hit_box());

    if (!collisions.empty()) {
        if (vertical_speed > 0) {
            // Hit ground
            vertical_speed = 0.0f;
            y = static_cast<int>(y / 16) * 16;
        }
    }

    // Check for edges or walls to turn around
    sf::FloatRect front_check(x + (horizontal_speed > 0 ? 16 : -16), y + 16, 16, 16);
    auto front_collisions = map_manager.map_collision(solid_cells, front_check);

    if (front_collisions.empty()) {
        // No ground ahead, turn around
        horizontal_speed *= -1;
    }

    // Wall collision
    sf::FloatRect wall_check(x + (horizontal_speed > 0 ? 16 : -16), y, 16, 16);
    auto wall_collisions = map_manager.map_collision(solid_cells, wall_check);

    if (!wall_collisions.empty()) {
        horizontal_speed *= -1;
    }

    // Check collision with Ray
    if (get_hit_box().intersects(ray.get_hit_box())) {
        if (ray.get_vertical_speed() > 0 && ray.get_y() < y) {
            // Ray is falling and above the ant - stomp kill
            die(0);
            ray.set_vertical_speed(-3.0f); // Bounce ray up
        }
        else {
            // Ray takes damage
            ray.die(false);
        }
    }

    // Update animation
    if (horizontal_speed != 0) {
        walk_animation.update();
    }

    // Remove if too far from view
    if (x < view_x - 100 || x > view_x + 420) {
        dead = true;
    }
}
