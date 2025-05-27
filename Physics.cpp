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

    // Clamp grid bounds to valid range
    topLeft.x = std::max(0, topLeft.x);
    topLeft.y = std::max(0, topLeft.y);
    bottomRight.x = std::min((int)map.grid[0].size() - 1, bottomRight.x);
    bottomRight.y = std::min((int)map.grid.size() - 1, bottomRight.y);

    // Check all tiles within bounds
    for (int gridY = topLeft.y; gridY <= bottomRight.y; ++gridY)
    {
        for (int gridX = topLeft.x; gridX <= bottomRight.x; ++gridX)
        {
            int tileType = map.grid[gridY][gridX];
            if (IsSolidTile(tileType))
            {
                return true;
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
    sf::Vector2f position = player.getPosition();

    // Store original position
    sf::Vector2f originalPosition = position;

    // Apply velocity to position
    position.x += velocity.x;
    position.y += velocity.y;

    // Check horizontal collision
    sf::FloatRect horizontalBounds = playerBounds;
    horizontalBounds.left = position.x - playerBounds.width / 2.0f;

    if (CheckCollision(horizontalBounds, map))
    {
        position.x = originalPosition.x; // Reset horizontal position
        player.setVelocityX(0);
    }

    // Check vertical collision
    sf::FloatRect verticalBounds = playerBounds;
    verticalBounds.top = position.y - playerBounds.height / 2.0f;
    verticalBounds.left = position.x - playerBounds.width / 2.0f;

    if (CheckCollision(verticalBounds, map))
    {
        if (velocity.y > 0) // Falling
        {
            player.setIsOnGround(true);
            player.setCanJump(true);

            // Snap to ground level
            sf::Vector2i gridPos = WorldToGrid(sf::Vector2f(position.x, position.y + playerBounds.height / 2.0f), map.cellSize);
            float groundLevel = GridToWorld(gridPos.x, gridPos.y, map.cellSize).y - map.cellSize / 2.0f;
            position.y = groundLevel - playerBounds.height / 2.0f;
        }
        else if (velocity.y < 0) // Jumping/moving up
        {
            // Snap to ceiling
            sf::Vector2i gridPos = WorldToGrid(sf::Vector2f(position.x, position.y - playerBounds.height / 2.0f), map.cellSize);
            float ceilingLevel = GridToWorld(gridPos.x, gridPos.y, map.cellSize).y + map.cellSize / 2.0f;
            position.y = ceilingLevel + playerBounds.height / 2.0f;
        }
        player.setVelocityY(0);
    }
    else
    {
        player.setIsOnGround(false);
        player.setIsOnPlatform(false);
    }

    // Check platform collision (one-way platforms)
    if (velocity.y > 0 && !player.getIsOnGround()) // Only when falling and not already on ground
    {
        sf::Vector2f bottomCenter(position.x, position.y + playerBounds.height / 2.0f);
        sf::Vector2i gridPos = WorldToGrid(bottomCenter, map.cellSize);

        if (gridPos.y >= 0 && gridPos.y < (int)map.grid.size() &&
            gridPos.x >= 0 && gridPos.x < (int)map.grid[0].size())
        {
            int tileType = map.grid[gridPos.y][gridPos.x];
            if (IsPlatformTile(tileType))
            {
                // Check if player was above the platform
                sf::Vector2f platformWorldPos = GridToWorld(gridPos.x, gridPos.y, map.cellSize);
                float platformTop = platformWorldPos.y - map.cellSize / 2.0f;

                // Check if player's bottom was above platform and is now intersecting
                float playerBottom = originalPosition.y + playerBounds.height / 2.0f;
                if (playerBottom <= platformTop && bottomCenter.y >= platformTop)
                {
                    player.setIsOnPlatform(true);
                    player.setIsOnGround(true);
                    player.setCanJump(true);
                    player.setVelocityY(0);
                    position.y = platformTop - playerBounds.height / 2.0f;
                }
            }
        }
    }

    // Update player position
    player.setPosition(position.x, position.y);
}

sf::Vector2i Physics::WorldToGrid(const sf::Vector2f& worldPos, float cellSize)
{
    // Convert world coordinates to grid coordinates
    // Assuming grid is centered at world origin
    int gridX = (int)std::floor((worldPos.x + 1600) / cellSize);
    int gridY = (int)std::floor((worldPos.y + 960) / cellSize);

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