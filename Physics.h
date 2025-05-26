#pragma once
#include "Player.h"
#include "Map.h"
#include "LevelEditor.h"

struct CollisionInfo {
    bool hasCollision;
    bool isGrounded;
    bool isOnPlatform;
    bool isOnLadder;
    bool takeDamage;
    sf::Vector2f correctionVector;
    TileType tileType;
};

class Physics {
public:
    static CollisionInfo CheckCollision(Player& player, const Map& map);
    static CollisionInfo CheckTileCollision(const sf::FloatRect& bounds, const Map& map, int tileX, int tileY);
    static bool CheckPlatformCollision(const sf::FloatRect& playerBounds, const sf::FloatRect& tileBounds, const sf::Vector2f& velocity);
    static sf::Vector2f GetTilePosition(int tileX, int tileY, float cellSize);
    static sf::FloatRect GetTileBounds(int tileX, int tileY, float cellSize);

    // Funciones de utilidad para diferentes tipos de colisión
    static bool IsPointInTile(const sf::Vector2f& point, const Map& map);
    static TileType GetTileAtPosition(const sf::Vector2f& position, const Map& map);
    static std::vector<sf::Vector2i> GetTilesInBounds(const sf::FloatRect& bounds, const Map& map);
    static sf::Vector2f CalculateCollisionCorrection(const sf::FloatRect& bounds, const sf::FloatRect& tileBounds);

    // Constantes de física
    static constexpr float PLATFORM_TOLERANCE = 5.0f; // Tolerancia para atravesar plataformas
};

// Implementación inline de funciones pequeñas
inline sf::Vector2f Physics::GetTilePosition(int tileX, int tileY, float cellSize) {
    return sf::Vector2f(tileX * cellSize + cellSize / 2, tileY * cellSize + cellSize / 2);
}

inline sf::FloatRect Physics::GetTileBounds(int tileX, int tileY, float cellSize) {
    return sf::FloatRect(tileX * cellSize, tileY * cellSize, cellSize, cellSize);
}

inline TileType Physics::GetTileAtPosition(const sf::Vector2f& position, const Map& map) {
    int tileX = static_cast<int>(position.x / map.cellSize);
    int tileY = static_cast<int>(position.y / map.cellSize);

    if (tileX >= 0 && tileX < static_cast<int>(map.grid[0].size()) &&
        tileY >= 0 && tileY < static_cast<int>(map.grid.size())) {
        return static_cast<TileType>(map.grid[tileY][tileX]);
    }

    return TileType::EMPTY;
}