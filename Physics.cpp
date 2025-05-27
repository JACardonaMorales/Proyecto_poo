#include "stdafx.h"
#include "Physics.h"
#include "LevelEditor.h"

bool Physics::CheckCollision(const sf::FloatRect& bounds, const Map& map)
{
    return CheckCollisionAt(bounds.left, bounds.top, bounds.width, bounds.height, map);
}

bool Physics::CheckCollisionAt(float x, float y, float width, float height, const Map& map)
{
    if (map.grid.empty()) return false;

    // Calculate grid bounds
    sf::Vector2i topLeft = WorldToGrid(sf::Vector2f(x, y), map.cellSize);
    sf::Vector2i bottomRight = WorldToGrid(sf::Vector2f(x + width, y + height), map.cellSize);

    // Check all tiles within bounds
    for (int gridY = topLeft.y; gridY <= bottomRight.y; ++gridY)
    {
        for (int gridX = topLeft.x; gridX <= bottomRight.x; ++gridX)
        {
            if (gridY >= 0 && gridY < (int)map.grid.size() &&
                gridX >= 0 && gridX < (int)map.grid[0].size())
            {
                int tileType = map.grid[gridY][gridX];
                if (IsSolidTile(tileType))
                {
                    return true;
                }
            }
        }
    }

    return false;
}

void Physics::HandlePlayerCollision(Player& player, const Map& map)
{
    if (map.grid.empty()) return;

    sf::FloatRect playerBounds = player.getGlobalBounds();
    sf::Vector2f velocity = player.getVelocity();

    // Check horizontal collision
    sf::FloatRect horizontalBounds = playerBounds;
    horizontalBounds.left += velocity.x;

    if (CheckCollision(horizontalBounds, map))
    {
        player.setVelocityX(0);
    }

    // Check vertical collision
    sf::FloatRect verticalBounds = playerBounds;
    verticalBounds.top += velocity.y;

    if (CheckCollision(verticalBounds, map))
    {
        if (velocity.y > 0) // Falling
        {
            player.setIsOnGround(true);
            player.setCanJump(true);
        }
        player.setVelocityY(0);
    }
    else
    {
        player.setIsOnGround(false);
    }

    // Check platform collision (one-way platforms)
    if (velocity.y > 0) // Only when falling
    {
        sf::Vector2i bottomCenter = WorldToGrid(
            sf::Vector2f(playerBounds.left + playerBounds.width / 2,
                playerBounds.top + playerBounds.height + velocity.y),
            map.cellSize
        );

        if (bottomCenter.y >= 0 && bottomCenter.y < (int)map.grid.size() &&
            bottomCenter.x >= 0 && bottomCenter.x < (int)map.grid[0].size())
        {
            int tileType = map.grid[bottomCenter.y][bottomCenter.x];
            if (IsPlatformTile(tileType))
            {
                // Check if player was above the platform
                float platformTop = (bottomCenter.y * map.cellSize) - (map.grid.size() * map.cellSize) / 2.0f;
                if (playerBounds.top + playerBounds.height <= platformTop + 5.0f) // Small tolerance
                {
                    player.setIsOnPlatform(true);
                    player.setIsOnGround(true);
                    player.setCanJump(true);
                    player.setVelocityY(0);
                    player.setPosition(player.getPosition().x, platformTop - playerBounds.height);
                }
            }
        }
    }
}

sf::Vector2i Physics::WorldToGrid(const sf::Vector2f& worldPos, float cellSize)
{
    // Convert world coordinates to grid coordinates
    // Assuming grid is centered at world origin
    int gridX = (int)((worldPos.x + 1600) / cellSize); // Assuming map is 50*32 = 1600 units wide
    int gridY = (int)((worldPos.y + 960) / cellSize);  // Assuming map is 30*32 = 960 units tall

    return sf::Vector2i(gridX, gridY);
}

sf::Vector2f Physics::GridToWorld(int gridX, int gridY, float cellSize)
{
    // Convert grid coordinates to world coordinates
    float worldX = gridX * cellSize - 1600 + cellSize / 2.0f;
    float worldY = gridY * cellSize - 960 + cellSize / 2.0f;

    return sf::Vector2f(worldX, worldY);
}

bool Physics::IsSolidTile(int tileType)
{
    TileType type = static_cast<TileType>(tileType);

    switch (type)
    {
    case TileType::WALL:
    case TileType::SPIKES:
    case TileType::DOOR:
    case TileType::BLOCK_TOP_LEFT:
    case TileType::BLOCK_TOP:
    case TileType::BLOCK_TOP_RIGHT:
    case TileType::BLOCK_LEFT:
    case TileType::BLOCK_CENTER:
    case TileType::BLOCK_RIGHT:
    case TileType::BLOCK_BOTTOM_LEFT:
    case TileType::BLOCK_BOTTOM:
    case TileType::BLOCK_BOTTOM_RIGHT:
        return true;
    default:
        return false;
    }
}

bool Physics::IsPlatformTile(int tileType)
{
    TileType type = static_cast<TileType>(tileType);
    return type == TileType::PLATFORM;
}