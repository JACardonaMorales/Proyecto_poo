#pragma once


#include <SFML/Graphics.hpp>

class MapManager;

class Life {
private:
    bool dead;
    char horizontal_direction;
    float vertical_speed;
    float x, y;
    unsigned short start_y;
    sf::Sprite sprite;
    sf::Texture texture;

public:
    Life(float x, float y);
    bool get_used() const;
    void draw(float view_x, sf::RenderWindow& window);
    void set_used(bool value);
    void update(float view_x, MapManager& map_manager);
    sf::FloatRect get_hit_box() const;
};
