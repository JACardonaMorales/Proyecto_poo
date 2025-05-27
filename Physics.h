#pragma once
#include <SFML/Graphics.hpp>
#include "Map.h"
#include "Player.h"

class Physics
{
public:
    static bool CheckCollision(const sf::FloatRect& bounds, const Map& map);
    static bool CheckCollisionAt(float x, float y, float width, float height, const Map& map);
    static void HandlePlayerCollision(Player& player, const Map& map);
    static sf::Vector2i WorldToGrid(const sf::Vector2f& worldPos, float cellSize);
    static sf::Vector2f GridToWorld(int gridX, int gridY, float cellSize);

private:
    static bool IsSolidTile(int tileType);
    static bool IsPlatformTile(int tileType);
};