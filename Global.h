#pragma once
#include <SFML/Graphics.hpp> 
#include <vector>
#include <memory>
#include <iostream> 
#include <fstream> 
#include <string>
#include <algorithm> 


class MapManager;
class Ray;
class Enemy;

const unsigned int SCREEN_WIDTH = 320;  
const unsigned int SCREEN_HEIGHT = 240;
const unsigned int WINDOW_SCALE = 3; // Escala de la ventana

const unsigned int CELL_SIZE = 16;

// Cell types for the map
enum class Cell : unsigned char {
    Empty = 0,
    Wall = 1,   
    Grass = 2,  // Para lógica de Infected_Ant, etc.
    Light = 3, 
    Life = 4,  
    Entrance = 5,
    Prock = 6,  
    Spikes = 7,
    Door = 8,
    Ground = 9 // Añadido para compatibilidad con DrawMap.cpp si se mantiene esa lógica.

    
};

// Map type definition
using Map = std::vector<std::vector<Cell>>;

// Función para cargar el mapa y entidades desde un archivo de texto (reemplaza a convert_sketch si es la nueva forma)
sf::Color convert_sketch_from_text(int level, bool& level_finish, // level_finish no se usa en la implementación de Global.cpp
    std::vector<std::shared_ptr<Enemy>>& enemies,
    sf::Color bg_color, MapManager& map_manager, Ray& ray);

// Función original para compatibilidad con imágenes (si aún es necesaria)
sf::Color convert_sketch(int level, bool level_finish, // level_finish no se usa
    std::vector<std::shared_ptr<Enemy>>& enemies,
    sf::Color bg_color, MapManager& map_manager, Ray& ray);

// Esta función draw_map de Global.cpp parece una versión alternativa/antigua.
// MapManager::draw_map y la de DrawMap.cpp son las otras.
// Si esta no se usa, considera eliminarla. Si se usa, asegúrate que su propósito es claro.
void draw_map(float view_x, const sf::Image& map_sketch, sf::RenderWindow& window,
    const sf::Texture& map_texture, const std::vector<std::vector<Cell>>& map_data); // Renombrado 'map' a 'map_data'

// Función de colisión genérica (si es diferente de MapManager::map_collision)
// Devuelve un índice de check_cells o 255 si no hay colisión.
unsigned char map_collision(float x, float y, const std::vector<Cell>& check_cells,
    const std::vector<std::vector<Cell>>& map_data); // Renombrado 'map' a 'map_data'
