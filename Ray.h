#pragma once

#include "Global.h"
#include <SFML/Graphics.hpp>
#include <vector>
#include "Animation.h"
#include "Life.h"

class MapManager;

class Ray {
private:
    int lifes;
    bool dead;
    bool flipped;
    bool on_ground;
    float enemy_bounce_speed;
    float horizontal_speed;
    float vertical_speed;
    float x, y;
    unsigned char jump_timer;
    unsigned char powerup_state;
    unsigned short death_timer;
    unsigned short life_take_timer;
    unsigned short invincible_timer;
    std::vector<Life> life_objects;
    sf::Sprite sprite;
    sf::Texture texture;
    Animation walk_animation;

public:
    Ray();
    bool get_dead() const;
    float get_vertical_speed() const;
    float get_x() const;
    float get_y() const;
    int get_lifes() const;
    void set_lifes(int value);
    void die(bool instant_death = false);
    void draw(sf::RenderWindow& window);
    void draw_leafs(float view_x, sf::RenderWindow& window);
    void reset();
    void set_position(float x, float y);
    void set_vertical_speed(float value);
    void update(float view_x, MapManager& map_manager);
    sf::FloatRect get_hit_box() const;
};
