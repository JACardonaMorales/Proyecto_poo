#pragma once
#include "Global.h"
#include "Animation.h"

class MapManager {
private:
    sf::Image map_sketch;
    sf::Sprite cell_sprite;
    sf::Texture map_texture;
    sf::Texture spikes_texture;
    sf::Texture door_texture;
    Map map;

    // Animations and effects


public:
    MapManager();

    // Map size functions
    unsigned short get_map_sketch_height() const;
    unsigned short get_map_sketch_width() const;
    unsigned short get_map_width() const;
    unsigned short get_map_height() const;

    // Map loading functions
    void MapManager::load_map_from_text(int level);
    void update_map_sketch(int current_level);
    sf::Color get_map_sketch_pixel(int x, int y) const;

    // Map manipulation
    void draw_map(bool draw_background, int level, float view_x, sf::RenderWindow& window);
    void set_map_cell(int x, int y, Cell cell);
    void set_map_size(unsigned int new_size);

    // Collision detection
    std::vector<unsigned char> map_collision(const std::vector<Cell>& check_cells, const sf::FloatRect& hitbox);
    std::vector<unsigned char> map_collision(const std::vector<Cell>& check_cells,
        const std::vector<Cell>& collision_cells,
        const sf::FloatRect& hitbox);

    // Effects
};
