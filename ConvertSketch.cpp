#include <array>
#include <cmath>
#include <chrono>
#include <SFML/Graphics.hpp>

#include "MapManager.h"
#include "ConvertSketch.h"
#include "Global.h"

void convert_sketch(const unsigned char i_current_level, MapManager& i_map_manager)
{
	unsigned short map_height;

	i_map_manager.update_map_sketch(i_current_level);
	i_map_manager.set_map_size(i_map_manager.get_map_sketch_width());

	// Dividimos por 2 porque ahora solo tenemos 2 capas: bloques y entidades
	map_height = floor(i_map_manager.get_map_sketch_height() / 2.f);

	for (unsigned short a = 0; a < i_map_manager.get_map_sketch_width(); a++)
	{
		for (unsigned short b = 0; b < map_height; b++)
		{
			sf::Color pixel = i_map_manager.get_map_sketch_pixel(a, b);

			// Capa de bloques (primera mitad de la imagen)
			if (sf::Color(139, 69, 19) == pixel) // Color marrón para tierra
			{
				i_map_manager.set_map_cell(a, b, Cell::Ground);
			}
			else if (sf::Color(101, 67, 33) == pixel) // Color marrón más oscuro para puerta
			{
				i_map_manager.set_map_cell(a, b, Cell::Door);
			}
			else if (sf::Color(255, 0, 0) == pixel) // Color rojo para espinas
			{
				i_map_manager.set_map_cell(a, b, Cell::Spikes);
			}
			else
			{
				i_map_manager.set_map_cell(a, b, Cell::Empty);
			}
		}

		// Segunda capa para entidades (segunda mitad de la imagen)
		for (unsigned short b = map_height; b < 2 * map_height; b++)
		{
			sf::Color pixel = i_map_manager.get_map_sketch_pixel(a, b);

			// Aquí podrías agregar spawn points de jugador u otras entidades
			// Por ejemplo:
			// if (sf::Color(0, 255, 0) == pixel) // Verde para spawn del jugador
			// {
			//     // Establecer posición inicial del jugador
			// }
		}
	}
}
