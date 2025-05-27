#pragma once


#include <cstdlib>
#include <ctime>
#include <time.h>
#include <string>
#include <vector>
#include <unordered_map>
#include <SFML/Audio.hpp>
#include <SFML/Network.hpp>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <iostream>
#include <filesystem>
#include <initializer_list>
#include <fstream>
#include <sstream>

// Constantes del juego
constexpr unsigned short CELL_SIZE = 32; // Tamaño de cada celda en píxeles
constexpr unsigned short SCREEN_WIDTH = 640;
constexpr unsigned short SCREEN_HEIGHT = 480;

// Enumeración para los tipos de celdas
enum class Cell
{
	Empty,
	Ground,    // Tierra
	Door,      // Puerta
	Spikes,		// Espinas
	Torch_Animation, // Animación de antorcha
	
};

// Tipo de mapa
typedef std::vector<std::vector<Cell>> Map;

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

