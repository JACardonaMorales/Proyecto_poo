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
    Spikes = 7,
    Door = 8
};

// Map type definition
using Map = std::vector<std::vector<Cell>>;

// Object class for particles and effects
class Object {
public:
    float horizontal_speed;
    float vertical_speed;
    float x, y;

    Object(float x, float y, float horizontal_speed, float vertical_speed);
};

// Function prototypes - REMOVED convert_sketch since we're not using it
// Removed: sf::Color convert_sketch(int level, bool level_finish, std::vector<std::shared_ptr<Enemy>>