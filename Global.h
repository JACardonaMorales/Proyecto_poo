#pragma once

#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>
#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>

// Forward declarations
class MapManager;
class Ray;
class Enemy;

// Cell types for the map
enum class Cell : unsigned char {
    Empty = 0,
    Wall = 1,
    Grass = 2,
    Light = 3,
    Life = 4,
    Entrance = 5,
    Prock = 6,
    Spikes = 7,    // Nueva celda para pinchos
    Door = 8       // Nueva celda para puerta
};

// Map type definition
using Map = std::vector<std::vector<Cell>>;

// Object class for particles and effects
class Object {
public:
    float horizontal_speed;
    float vertical_speed;
    float x, y;

    Object(float x, float y, float horizontal_speed, float vertical_speed)
        : x(x), y(y), horizontal_speed(horizontal_speed), vertical_speed(vertical_speed) {
    }
};

// Function prototypes
sf::Color convert_sketch(int level, bool level_finish, std::vector<std::shared_ptr<Enemy>>& enemies,
    sf::Color bg_color, MapManager& map_manager, Ray& ray);

void draw_map(float view_x, const sf::Image& map_sketch, sf::RenderWindow& window,
    const sf::Texture& map_texture, const std::vector<std::vector<Cell>>& map);

unsigned char map_collision(float x, float y, const std::vector<Cell>& check_cells,
    const std::vector<std::vector<Cell>>& map);