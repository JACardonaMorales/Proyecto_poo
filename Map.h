#pragma once
#include "Renderer.h"

class Map
{
public:
	Map(float cellSize = 32.0f);
	void createBoard(size_t width, size_t height);
	void Draw(Renderer& renderer);
	void InitFromImage(const sf::Image& image);

	std::vector<std::vector<int>> grid;
	float cellSize;

private:
	void CreateDefaultTextures();
};



