#ifndef GLOBAL_H
#define GLOBAL_H

#include <SFML/Graphics.hpp>
#include <vector>

enum class Cell {
    Light,
    Grass,
    Life,
    Empty,
    Entrance,
    Prock,
    Wall
};

// Forward declarations
class MapManager;
class Ray;
class Enemy;

// Global utility functions
sf::Color convert_sketch(int level, bool level_finish, std::vector<std::shared_ptr<Enemy>>& enemies,
    sf::Color bg_color, MapManager& map_manager, Ray& ray);

void draw_map(float view_x, const sf::Image& map_sketch, sf::RenderWindow& window,
    const sf::Texture& map_texture, const std::vector<std::vector<Cell>>& map);

unsigned char map_collision(float x, float y, const std::vector<Cell>& check_cells,
    const std::vector<std::vector<Cell>>& map);

#endif