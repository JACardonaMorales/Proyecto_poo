#include <array>
#include <chrono>
#include <cmath>
#include <SFML/Graphics.hpp>

#include "Global.h"
#include "DrawMap.h"
#include "MapManager.h"

void draw_map(unsigned i_view_x, sf::RenderWindow& i_window, const sf::Texture& i_map_texture, const Map& i_map, unsigned short map_height)
{
	// Solo dibujamos la parte visible en pantalla
	unsigned short map_end = ceil((i_view_x + SCREEN_WIDTH) / static_cast<float>(CELL_SIZE));
	unsigned short map_start = floor(i_view_x / static_cast<float>(CELL_SIZE));

	sf::Sprite cell_sprite(i_map_texture);

	for (unsigned short a = map_start; a < map_end; a++)
	{
		for (unsigned short b = 0; b < map_height; b++)
		{
			if (Cell::Empty != i_map[a][b])
			{
				unsigned short sprite_x = 0;
				unsigned short sprite_y = 0;

				cell_sprite.setPosition(CELL_SIZE * a, CELL_SIZE * b);

				// Determinar qué sprite usar basado en el tipo de celda
				if (Cell::Ground == i_map[a][b])
				{
					// Determinar qué tipo de bloque de tierra dibujar basándose en los vecinos
					bool has_left = (a > 0 && Cell::Ground == i_map[a - 1][b]);
					bool has_right = (a < i_map.size() - 1 && Cell::Ground == i_map[a + 1][b]);
					bool has_up = (b > 0 && Cell::Ground == i_map[a][b - 1]);
					bool has_down = (b < map_height - 1 && Cell::Ground == i_map[a][b + 1]);

					// Lógica para seleccionar el sprite correcto del tileset
					if (!has_up && has_down) // Superficie superior
					{
						if (!has_left && !has_right) // Bloque aislado arriba
						{
							sprite_x = 1; sprite_y = 0; // Superficie sola
						}
						else if (!has_left && has_right) // Esquina superior izquierda
						{
							sprite_x = 0; sprite_y = 0;
						}
						else if (has_left && !has_right) // Esquina superior derecha
						{
							sprite_x = 2; sprite_y = 0;
						}
						else // Superficie intermedia
						{
							sprite_x = 1; sprite_y = 0;
						}
					}
					else if (has_up && has_down) // Bloque intermedio vertical
					{
						if (!has_left && has_right) // Borde izquierdo
						{
							sprite_x = 0; sprite_y = 1;
						}
						else if (has_left && !has_right) // Borde derecho
						{
							sprite_x = 2; sprite_y = 1;
						}
						else // Centro
						{
							sprite_x = 1; sprite_y = 1;
						}
					}
					else if (has_up && !has_down) // Superficie inferior
					{
						if (!has_left && has_right) // Esquina inferior izquierda
						{
							sprite_x = 0; sprite_y = 2;
						}
						else if (has_left && !has_right) // Esquina inferior derecha
						{
							sprite_x = 2; sprite_y = 2;
						}
						else // Superficie inferior intermedia
						{
							sprite_x = 1; sprite_y = 2;
						}
					}
					else // Bloque completamente aislado
					{
						sprite_x = 1; sprite_y = 1; // Usar sprite central
					}
				}
				else if (Cell::Door == i_map[a][b])
				{
					sprite_x = 3; // Posición de la puerta en el tileset
					sprite_y = 0;
				}
				else if (Cell::Spikes == i_map[a][b])
				{
					sprite_x = 0; // Posición de las espinas en el tileset
					sprite_y = 3;
				}

				cell_sprite.setTextureRect(sf::IntRect(CELL_SIZE * sprite_x, CELL_SIZE * sprite_y, CELL_SIZE, CELL_SIZE));
				i_window.draw(cell_sprite);
			}
		}
	}
}
