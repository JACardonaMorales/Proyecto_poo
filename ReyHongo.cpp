#include "stdafx.h"
#include "ReyHongo.h"
#include "MapManager.h"
#include "Ray.h"

ReyHongo::ReyHongo(bool attacks, float x, float y)
    : Enemy(x, y),
    get_out_animation(16, "textures/rey_hongo_emerge.png", 8),
    walk_animation(16, "textures/rey_hongo_walk.png", 10),
    attacks(attacks), check_collision(true), flipped(false),
    no_collision_dying(false), state(0), get_out_timer(0) {

    if (!texture.loadFromFile("textures/rey_hongo.png")) {
        // Handle error
    }
    sprite.setTexture(texture);

    horizontal_speed = attacks ? 1.2f : 0.8f;
}

bool ReyHongo::get_dead(bool deletion) {
    if (deletion && state == 3) { // Dead state
        return true;
    }
    return dead;
}

void ReyHongo::die(int death_type) {
    if (!dead && state != 3) {
        if (death_type == 0) { // Stomp kill
            dead = true;
            state = 3;
            vertical_speed = -4.0f;
            no_collision_dying = true;
        }
        else if (death_type == 1) { // Fire kill
            dead = true;
            state = 3;
        }
    }
}

void ReyHongo::draw(float view_x, sf::RenderWindow& window) {
    if (dead && state == 3) {
        // Draw dead sprite upside down
        sprite.setPosition(x - view_x, y);
        sprite.setScale(flipped ? -1.0f : 1.0f, -1.0f);
        window.draw(sprite);
        return;
    }

    switch (state) {
    case 0: // Emerging
        get_out_animation.set_position(x - view_x, y);
        get_out_animation.set_flipped(flipped);
        get_out_animation.draw(window);
        break;

    case 1: // Walking
        if (horizontal_speed != 0) {
            walk_animation.set_position(x - view_x, y);
            walk_animation.set_flipped(flipped);
            walk_animation.draw(window);
        }
        else {
            sprite.setPosition(x - view_x, y);
            sprite.setScale(flipped ? -1.0f : 1.0f, 1.0f);
            window.draw(sprite);
        }
        break;

    case 2: // Hidden
        // Don't draw when hidden
        break;
    }
}

void ReyHongo::update(float view_x, std::vector<std::shared_ptr<Enemy>>& enemies,
    MapManager& map_manager, Ray& ray) {
    if (dead && state == 3) {
        if (no_collision_dying) {
            vertical_speed += 0.2f;
            y += vertical_speed;
        }
        return;
    }

    switch (state) {
    case 0:
    {
        // Emerging
        get_out_timer++;
        get_out_animation.update();
        if (get_out_timer >= 60) { // 1 second
            state = 1;
            get_out_timer = 0;
        }
        break;
    }
    case 1:
    {
        // Walking
    // Movement
        x += horizontal_speed;
        flipped = horizontal_speed < 0;

        // Aplica gravity
        vertical_speed += 0.2f;
        y += vertical_speed;

        // Check collisions with map
        if (check_collision) {
            std::vector<Cell> solid_cells = { Cell::Wall, Cell::Grass, Cell::Prock };
            auto collisions = map_manager.map_collision(solid_cells, get_hit_box());

            if (!collisions.empty()) {
                if (vertical_speed > 0) {
                    vertical_speed = 0.0f;
                    y = static_cast<int>(y / 16) * 16;
                }
            }

            // Check for edges or walls
            sf::FloatRect front_check(x + (horizontal_speed > 0 ? 16 : -16), y + 16, 16, 16);
            auto front_collisions = map_manager.map_collision(solid_cells, front_check);

            if (front_collisions.empty()) {
                horizontal_speed *= -1;
            }

            sf::FloatRect wall_check(x + (horizontal_speed > 0 ? 16 : -16), y, 16, 16);
            auto wall_collisions = map_manager.map_collision(solid_cells, wall_check);

            if (!wall_collisions.empty()) {
                horizontal_speed *= -1;
            }
        }

        // Check collision with Ray
        if (get_hit_box().intersects(ray.get_hit_box())) {
            if (ray.get_vertical_speed() > 0 && ray.get_y() < y) {
                // Stomp kill
                die(0);
                ray.set_vertical_speed(-3.0f);
            }
            else {
                // Ray takes damage
                ray.die(false);
            }
        }

        // Update walking animation
        walk_animation.update();

        // Hide if Ray is too far
        float distance = abs(ray.get_x() - x);
        // Rename the local variable to avoid shadowing the member variable 'distance'
        float ray_distance = abs(ray.get_x() - x);
        if (distance > 100) {
            state = 2;
            get_out_timer = 0;
        }
        break;
    }
    case 2: // Escondido
    {
        get_out_timer++;
        // Re-emerge si Ray se acerca
        float distance = abs(ray.get_x() - x);
        if (distance < 80 || get_out_timer > 300) { // 5 segundos
            state = 0;
            get_out_timer = 0;
        }
        break;
    }
    // Remove if too far from view
    if (x < view_x - 100 || x > view_x + 420) {
        dead = true;
    }
    }
}
        
    

