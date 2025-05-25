#include "stdafx.h"
#include "Game.h"

Map::Map(float cellSize)
	: cellSize(cellSize), grid()
{
}

void Map::createBoard(size_t width, size_t height)
{
	grid = std::vector(width, std::vector(height, 0));

	bool last = 0;
	for (auto& column : grid)
	{
		for (auto& cell : column)
		{
			last = cell = !last;
		}
		if (width % 2 == 0)
		{
			last = !last;
		}
	}
}

void Map::Draw(Renderer& renderer)
{
	// OPTIMIZACIÓN: Solo renderizar lo que está en pantalla
	sf::Vector2f cameraPos = camera.position;
	sf::Vector2f cameraSize = sf::Vector2f(800, 600); // Tamaño aproximado de vista

	int startX = std::max(0, (int)((cameraPos.x - cameraSize.x / 2) / cellSize) - 1);
	int endX = std::min((int)grid.size(), (int)((cameraPos.x + cameraSize.x / 2) / cellSize) + 1);
	int startY = 0;
	int endY = grid.empty() ? 0 : std::min((int)grid[0].size(), (int)((cameraPos.y + cameraSize.y / 2) / cellSize) + 1);

	for (int x = startX; x < endX && x < (int)grid.size(); ++x)
	{
		for (int y = startY; y < endY && y < (int)grid[x].size(); ++y)
		{
			if (grid[x][y] != 0)
			{
				sf::Vector2f position(
					cellSize * x + cellSize / 2.0f,
					cellSize * y + cellSize / 2.0f  // ESTE ERA EL BUG - estaba multiplicando por cellSize dos veces
				);
				sf::Vector2f size(cellSize, cellSize);

				switch (grid[x][y]) {
				case 1: // Solid Block
					if (Resources::textures.find("Tile") != Resources::textures.end()) {
						renderer.Draw(Resources::textures["Tile"], position, size);
					}
					break;
				case 2: // Spike
					if (Resources::textures.find("Spike") != Resources::textures.end()) {
						renderer.Draw(Resources::textures["Spike"], position, size);
					}
					else if (Resources::textures.find("Tile") != Resources::textures.end()) {
						renderer.Draw(Resources::textures["Tile"], position, size);
					}
					break;
				case 3: // Door
					if (Resources::textures.find("Door") != Resources::textures.end()) {
						renderer.Draw(Resources::textures["Door"], position, size);
					}
					else if (Resources::textures.find("Tile") != Resources::textures.end()) {
						renderer.Draw(Resources::textures["Tile"], position, size);
					}
					break;
				}
			}
		}
	}
}

void Map::InitFromImage(const sf::Image& image)
{
	grid.clear();
	grid.resize(image.getSize().x, std::vector<int>(image.getSize().y, 0));
	for (size_t x = 0; x < image.getSize().x; ++x)
	{
		for (size_t y = 0; y < image.getSize().y; ++y)
		{
			sf::Color pixelColor = image.getPixel(x, y);
			if (pixelColor == sf::Color::Black)
			{
				grid[x][y] = 1; // Solid block for black pixels
			}
			else if (pixelColor == sf::Color::Red)
			{
				grid[x][y] = 2; // Spike for red pixels
			}
			else if (pixelColor == sf::Color::Blue)
			{
				grid[x][y] = 3; // Door for blue pixels
			}
			else
			{
				grid[x][y] = 0; // Empty for other colors
			}
		}
	}
}