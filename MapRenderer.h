// MapRenderer.h
#pragma once
#include "TileRenderer.h"
struct MapData {
    std::vector<std::vector<TileType>> tiles;
    int width;
    int height;
    float tileSize;

    MapData() : width(0), height(0), tileSize(32.0f) {}

    explicit MapData(int w, int h, float size = 32.0f)
        : width(w), height(h), tileSize(size) {
        tiles.resize(static_cast<size_t>(height), std::vector<TileType>(static_cast<size_t>(width), TileType::EMPTY));
    }
};

class TileRenderer;

class MapRenderer {
private:
    TileRenderer tileRenderer;
    MapData mapData;
    float animationTime;
    sf::Clock animationClock;

    // Optimization for large maps
    bool enableCulling;
    sf::FloatRect cullingBounds;

public:
    explicit MapRenderer(float tileSize = 32.0f);

    // Load map from file
    bool LoadMap(const std::string& filename);

    // Save map to file
    bool SaveMap(const std::string& filename) const;

    // Set map data directly
    void SetMapData(const MapData& data);
    void SetMapData(const std::vector<std::vector<TileType>>& tiles, int width, int height, float tileSize = 32.0f);

    // Get map data
    const MapData& GetMapData() const { return mapData; }

    // Rendering
    void Render(sf::RenderWindow& window, const Camera& camera);
    void RenderFullMap(sf::RenderWindow& window);

    // Utility functions
    sf::Vector2f GetWorldPosition(int gridX, int gridY) const;
    sf::Vector2i GetGridPosition(const sf::Vector2f& worldPos) const;
    TileType GetTileAt(int gridX, int gridY) const;
    void SetTileAt(int gridX, int gridY, TileType type);

    // Map properties
    sf::Vector2i GetMapSize() const { return sf::Vector2i(mapData.width, mapData.height); }
    float GetTileSize() const { return mapData.tileSize; }
    sf::FloatRect GetMapBounds() const;

    // Optimization
    void SetCullingEnabled(bool enabled) { enableCulling = enabled; }
    bool IsCullingEnabled() const { return enableCulling; }

    // Clear map
    void Clear();
    void Clear(int width, int height);
};