#pragma once
#include "Global.h"

class MapManager
{
	sf::Image map_sketch;
	sf::Sprite cell_sprite;
	sf::Texture map_texture;
	Map map;

public:
	MapManager();

	unsigned short get_map_sketch_height() const;
	unsigned short get_map_sketch_width() const;
	unsigned short get_map_width() const;

	void draw_map(const unsigned i_view_x, sf::RenderWindow& i_window);
	void set_map_cell(const unsigned short i_x, const unsigned short i_y, const Cell& i_cell);
	void set_map_size(const unsigned short i_new_size);
	void update_map_sketch(const unsigned char i_current_level);

	sf::Color get_map_sketch_pixel(const unsigned short i_x, const unsigned short i_y) const;
};