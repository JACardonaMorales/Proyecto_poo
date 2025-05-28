#pragma once
#include "Global.h" 
#include "Animation.h" 
#include <SFML/Graphics.hpp>
#include <vector>  
#include <string>   
#include <iostream>  


class MapManager {
private:
    sf::Sprite cell_sprite;
    sf::Texture map_texture;    // Tileset general
    sf::Texture spikes_texture; // Textura específica para pinchos
    sf::Texture door_texture;   // Textura específica para puertas
    Map game_map; // Renombrado de 'map' para evitar conflicto con std::map si se incluyera accidentalmente

public:
    MapManager();

    // Funciones del mapa principal (game_map)
    unsigned short get_map_width() const;    // Ancho en celdas
    unsigned short get_map_height() const;   // Alto en celdas

    void load_map_from_text(int level);
    void draw_map(bool draw_background, int level, float view_x, sf::RenderWindow& window);
    void set_map_cell(int x, int y, Cell cell);
    void set_map_size(unsigned int new_width, unsigned int new_height); // Permitir ancho y alto diferentes

    // Detección de colisiones
    // Devuelve un vector de los *tipos* de celdas con las que se colisionó (basado en check_cells)
    // o mejor, un vector de sf::Vector2i de las celdas colisionadas, o simplemente bool.
    // La versión actual devuelve índices, lo cual es un poco oscuro.
    std::vector<unsigned char> map_collision(const std::vector<Cell>& check_cells, const sf::FloatRect& hitbox);

    // Esta sobrecarga parece redundante ya que collision_cells no se usa en la implementación.
    // Considera eliminarla si no se va a usar de forma diferente.
    std::vector<unsigned char> map_collision(const std::vector<Cell>& check_cells,
        const std::vector<Cell>& collision_cells_ignored, // Renombrado para indicar que se ignora
        const sf::FloatRect& hitbox);

    // Getter para el mapa si es necesario externamente (con precaución)
    // const Map& get_current_map() const { return game_map; }
};