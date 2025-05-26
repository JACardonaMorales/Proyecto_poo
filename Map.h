#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include "LevelEditor.h"
#include "Renderer.h"

// Forward declarations
class LevelEditor;

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
	void CreatePlaceholderTextures();
	void DrawAnimatedTorch(Renderer& renderer, const sf::Vector2f& position, const sf::Vector2f& size, float animationTime);
	void DrawTilesetTile(Renderer& renderer, const sf::Vector2f& position, const sf::Vector2f& size, TileType tileType);
};