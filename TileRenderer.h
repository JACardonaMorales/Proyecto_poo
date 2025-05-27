#pragma once
#include "LevelEditor.h"
#include "Game.h"
#include "Resources.h"
#include <SFML/Graphics.hpp>
#include <vector>

class TileRenderer {
private:
    float tileSize;
    std::vector<TileInfo> tileInfos;

public:
    TileRenderer(float size = 32.0f) : tileSize(size) {
        InitializeTileInfos();
    }

    void InitializeTileInfos();

    void RenderTile(sf::RenderWindow& window, TileType type, const sf::Vector2f& position, float animTime = 0.0f);

    void RenderTorch(sf::RenderWindow& window, const sf::Vector2f& position, float animTime);

    sf::IntRect GetTilesetRect(TileType type) const;

    sf::Vector2f GetWorldPosition(int gridX, int gridY, int gridWidth, int gridHeight) const;
};