#include "stdafx.h"
#include "Game.h"


Map::Map(float cellSize)
	: cellSize(cellSize), grid()
{
	// Constructor implementation (if needed)
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
	int x = 0;
	for (const auto& column : grid)
	{
		int y = 0;
		for (const auto& cell : column)
		{
			if (cell)
			{
				renderer.Draw(Resources::textures["Tile"], 
					sf::Vector2f(cellSize * x + cellSize / 2.0f,
						cellSize * y * cellSize / 2.0f), 
					sf::Vector2f(cellSize, cellSize));
			}
			y++;
		}
		x++;
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
				grid[x][y] = 1; // Set the cell to 1 for black pixels
			}
			else
			{
				grid[x][y] = 0; // Set the cell to 0 for non-black pixels
			}
		}
	}
}