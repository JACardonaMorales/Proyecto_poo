#include "stdafx.h"

#include "MapRenderer.h"
#include <fstream>
#include <iostream>

MapRenderer::MapRenderer(float tileSize)
    : tileRenderer(tileSize), animationTime(0.0f), enableCulling(true) {
    animationClock.restart();
}

bool MapRenderer::LoadMap(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open map file: " << filename << std::endl;
        return false;
    }

    try {
        // Read map dimensions and tile size
        file.read(reinterpret_cast<char*>(&mapData.width), sizeof(int));
        file.read(reinterpret_cast<char*>(&mapData.height), sizeof(int));
        file.read(reinterpret_cast<char*>(&mapData.tileSize), sizeof(float));

        // Validate dimensions
        if (mapData.width <= 0 || mapData.height <= 0 ||
            mapData.width > 1000 || mapData.height > 1000) {
            std::cerr << "Error: Invalid map dimensions in file: " << filename << std::endl;
            return false;
        }

        // Resize and read tile data
        mapData.tiles.clear();
        mapData.tiles.resize(static_cast<size_t>(mapData.height), std::vector<TileType>(static_cast<size_t>(mapData.width)));

        for (int y = 0; y < mapData.height; ++y) {
            for (int x = 0; x < mapData.width; ++x) {
                int tileValue;
                file.read(reinterpret_cast<char*>(&tileValue), sizeof(int));
                mapData.tiles[static_cast<size_t>(y)][static_cast<size_t>(x)] = static_cast<TileType>(tileValue);
            }
        }

        // Update tile renderer size
        tileRenderer = TileRenderer(mapData.tileSize);

        std::cout << "Map loaded successfully: " << mapData.width << "x" << mapData.height
            << " (tile size: " << mapData.tileSize << ")" << std::endl;
        return true;

    }
    catch (const std::exception& e) {
        std::cerr << "Error reading map file: " << e.what() << std::endl;
        return false;
    }
}

bool MapRenderer::SaveMap(const std::string& filename) const {
    std::ofstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Error: Could not create map file: " << filename << std::endl;
        return false;
    }

    try {
        // Write map dimensions and tile size
        file.write(reinterpret_cast<const char*>(&mapData.width), sizeof(int));
        file.write(reinterpret_cast<const char*>(&mapData.height), sizeof(int));
        file.write(reinterpret_cast<const char*>(&mapData.tileSize), sizeof(float));

        // Write tile data
        for (int y = 0; y < mapData.height; ++y) {
            for (int x = 0; x < mapData.width; ++x) {
                int tileValue = static_cast<int>(mapData.tiles[static_cast<size_t>(y)][static_cast<size_t>(x)]);
                file.write(reinterpret_cast<const char*>(&tileValue), sizeof(int));
            }
        }

        std::cout << "Map saved successfully: " << filename << std::endl;
        return true;

    }
    catch (const std::exception& e) {
        std::cerr << "Error writing map file: " << e.what() << std::endl;
        return false;
    }
}

void MapRenderer::SetMapData(const MapData& data) {
    mapData = data;
    tileRenderer = TileRenderer(mapData.tileSize);
}

void MapRenderer::SetMapData(const std::vector<std::vector<TileType>>& tiles, int width, int height, float tileSize) {
    mapData.tiles = tiles;
    mapData.width = width;
    mapData.height = height;
    mapData.tileSize = tileSize;
    tileRenderer = TileRenderer(tileSize);
}

void MapRenderer::Render(sf::RenderWindow& window, const Camera& camera) {
    // Update animation time
    animationTime = animationClock.getElapsedTime().asSeconds();

    sf::View currentView = window.getView();
    sf::Vector2f viewCenter = currentView.getCenter();
    sf::Vector2f viewSize = currentView.getSize();

    // Calculate visible area for culling
    if (enableCulling) {
        cullingBounds.left = viewCenter.x - viewSize.x * 0.6f;
        cullingBounds.top = viewCenter.y - viewSize.y * 0.6f;
        cullingBounds.width = viewSize.x * 1.2f;
        cullingBounds.height = viewSize.y * 1.2f;
    }

    // Render tiles
    for (int y = 0; y < mapData.height; ++y) {
        for (int x = 0; x < mapData.width; ++x) {
            TileType tileType = mapData.tiles[static_cast<size_t>(y)][static_cast<size_t>(x)];

            if (tileType == TileType::EMPTY) {
                continue;
            }

            sf::Vector2f worldPos = GetWorldPosition(x, y);

            // Culling check
            if (enableCulling) {
                if (worldPos.x + mapData.tileSize < cullingBounds.left ||
                    worldPos.x > cullingBounds.left + cullingBounds.width ||
                    worldPos.y + mapData.tileSize < cullingBounds.top ||
                    worldPos.y > cullingBounds.top + cullingBounds.height) {
                    continue;
                }
            }

            // Render the tile
            if (tileType == TileType::TORCH) {
                tileRenderer.RenderTorch(window, worldPos, animationTime);
            }
            else {
                tileRenderer.RenderTile(window, tileType, worldPos, animationTime);
            }
        }
    }
}

void MapRenderer::RenderFullMap(sf::RenderWindow& window) {
    // Update animation time
    animationTime = animationClock.getElapsedTime().asSeconds();

    // Render all tiles without culling
    for (int y = 0; y < mapData.height; ++y) {
        for (int x = 0; x < mapData.width; ++x) {
            TileType tileType = mapData.tiles[static_cast<size_t>(y)][static_cast<size_t>(x)];

            if (tileType == TileType::EMPTY) {
                continue;
            }

            sf::Vector2f worldPos = GetWorldPosition(x, y);

            // Render the tile
            if (tileType == TileType::TORCH) {
                tileRenderer.RenderTorch(window, worldPos, animationTime);
            }
            else {
                tileRenderer.RenderTile(window, tileType, worldPos, animationTime);
            }
        }
    }
}

sf::Vector2f MapRenderer::GetWorldPosition(int gridX, int gridY) const {
    return tileRenderer.GetWorldPosition(gridX, gridY, mapData.width, mapData.height);
}

sf::Vector2i MapRenderer::GetGridPosition(const sf::Vector2f& worldPos) const {
    int gridX = static_cast<int>(std::floor((worldPos.x + static_cast<float>(mapData.width) * mapData.tileSize * 0.5f) / mapData.tileSize));
    int gridY = static_cast<int>(std::floor((worldPos.y + static_cast<float>(mapData.height) * mapData.tileSize * 0.5f) / mapData.tileSize));

    // Clamp to valid range
    gridX = std::max(0, std::min(gridX, mapData.width - 1));
    gridY = std::max(0, std::min(gridY, mapData.height - 1));

    return sf::Vector2i(gridX, gridY);
}

TileType MapRenderer::GetTileAt(int gridX, int gridY) const {
    if (gridX < 0 || gridX >= mapData.width || gridY < 0 || gridY >= mapData.height) {
        return TileType::EMPTY;
    }
    return mapData.tiles[static_cast<size_t>(gridY)][static_cast<size_t>(gridX)];
}

void MapRenderer::SetTileAt(int gridX, int gridY, TileType type) {
    if (gridX >= 0 && gridX < mapData.width && gridY >= 0 && gridY < mapData.height) {
        mapData.tiles[static_cast<size_t>(gridY)][static_cast<size_t>(gridX)] = type;
    }
}

sf::FloatRect MapRenderer::GetMapBounds() const {
    sf::Vector2f topLeft = GetWorldPosition(0, 0);
    sf::Vector2f bottomRight = GetWorldPosition(mapData.width - 1, mapData.height - 1);

    return sf::FloatRect(
        topLeft.x,
        topLeft.y,
        bottomRight.x - topLeft.x + mapData.tileSize,
        bottomRight.y - topLeft.y + mapData.tileSize
    );
}

void MapRenderer::Clear() {
    mapData.tiles.clear();
    mapData.width = 0;
    mapData.height = 0;
}

void MapRenderer::Clear(int width, int height) {
    mapData.width = width;
    mapData.height = height;
    mapData.tiles.clear();
    mapData.tiles.resize(static_cast<size_t>(height), std::vector<TileType>(static_cast<size_t>(width), TileType::EMPTY));
}