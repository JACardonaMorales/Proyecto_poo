#ifndef MAPMANAGER_H
#define MAPMANAGER_H

#include <SFML/Graphics.hpp>
#include <vector>
#include "Global.h"
#include "Animation.h"
#include "Object.h"

using Map = std::vector<std::vector<Cell>>;

class MapManager {
private:
    std::vector<Object> brick_particles;
    std::vector<Object> question_block_coins;
    sf::Image map_sketch;
    sf::Sprite cell_sprite;
    sf::Texture map_texture;
    Animation coin_animation;
    Animation question_block_animation;
    Map map;

public:
    MapManager();
    unsigned short get_map_sketch_height() const;
    unsigned short get_map_sketch_width() const;
    unsigned short get_map_width() const;
    void add_brick_particles(float x, float y);
    void add_question_block_coin(float x, float y);
    void draw_map(bool draw_background, int level, float view_x, sf::RenderWindow& window);
    void set_map_cell(int x, int y, Cell cell);
    void set_map_size(unsigned int new_size);
    void update();
    void update_map_sketch(int current_level);
    std::vector<unsigned char> map_collision(const std::vector<Cell>& check_cells,
        const sf::FloatRect& hitbox);
    std::vector<unsigned char> map_collision(const std::vector<Cell>& check_cells,
        const std::vector<Cell>& collision_cells,
        const sf::FloatRect& hitbox);
    sf::Color get_map_sketch_pixel(int x, int y) const;
    const Map& get_map() const { return map; }
};

#endif