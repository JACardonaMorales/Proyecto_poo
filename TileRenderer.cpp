#include "stdafx.h"
#include "TileRenderer.h"
#include "Resources.h"

void TileRenderer::InitializeTileInfos() {
    // Clear existing tile infos
    tileInfos.clear();

    // Initialize basic tiles
    tileInfos.emplace_back(TileType::EMPTY, false, false, sf::Vector2i(0, 0), "");
    tileInfos.emplace_back(TileType::WALL, true, false, sf::Vector2i(0, 0), "tileset");
    tileInfos.emplace_back(TileType::PLATFORM, true, false, sf::Vector2i(1, 0), "tileset");
    tileInfos.emplace_back(TileType::SPIKES, true, false, sf::Vector2i(2, 0), "tileset");
    tileInfos.emplace_back(TileType::TORCH, false, true, sf::Vector2i(3, 0), "tileset");
    tileInfos.emplace_back(TileType::DOOR, true, false, sf::Vector2i(4, 0), "tileset");

    // Initialize tileset blocks
    tileInfos.emplace_back(TileType::BLOCK_TOP_LEFT, true, false, sf::Vector2i(0, 1), "tileset");
    tileInfos.emplace_back(TileType::BLOCK_TOP, true, false, sf::Vector2i(1, 1), "tileset");
    tileInfos.emplace_back(TileType::BLOCK_TOP_RIGHT, true, false, sf::Vector2i(2, 1), "tileset");
    tileInfos.emplace_back(TileType::BLOCK_LEFT, true, false, sf::Vector2i(0, 2), "tileset");
    tileInfos.emplace_back(TileType::BLOCK_CENTER, true, false, sf::Vector2i(1, 2), "tileset");
    tileInfos.emplace_back(TileType::BLOCK_RIGHT, true, false, sf::Vector2i(2, 2), "tileset");
    tileInfos.emplace_back(TileType::BLOCK_BOTTOM_LEFT, true, false, sf::Vector2i(0, 3), "tileset");
    tileInfos.emplace_back(TileType::BLOCK_BOTTOM, true, false, sf::Vector2i(1, 3), "tileset");
    tileInfos.emplace_back(TileType::BLOCK_BOTTOM_RIGHT, true, false, sf::Vector2i(2, 3), "tileset");

    // Initialize shadow blocks
    tileInfos.emplace_back(TileType::BLOCK_SHADOW_LEFT, false, false, sf::Vector2i(3, 1), "tileset");
    tileInfos.emplace_back(TileType::BLOCK_SHADOW_RIGHT, false, false, sf::Vector2i(4, 1), "tileset");
    tileInfos.emplace_back(TileType::BLOCK_SHADOW_BOTTOM, false, false, sf::Vector2i(3, 2), "tileset");
    tileInfos.emplace_back(TileType::BLOCK_SHADOW_CORNER, false, false, sf::Vector2i(4, 2), "tileset");
}

void TileRenderer::RenderTile(sf::RenderWindow& window, TileType type, const sf::Vector2f& position, float animTime) {
    if (type == TileType::EMPTY) {
        return;
    }

    if (type == TileType::TORCH) {
        RenderTorch(window, position, animTime);
        return;
    }

    sf::Texture* texture = Resources::GetTexture("tileset");
    if (!texture) {
        return;
    }

    sf::Sprite sprite;
    sprite.setTexture(*texture);

    // Get the tile rectangle from tileset
    sf::IntRect tileRect = GetTilesetRect(type);
    sprite.setTextureRect(tileRect);

    // Set position and scale
    sprite.setPosition(position);

    // Scale to fit tile size
    float scaleX = tileSize / static_cast<float>(tileRect.width);
    float scaleY = tileSize / static_cast<float>(tileRect.height);
    sprite.setScale(scaleX, scaleY);

    window.draw(sprite);
}

void TileRenderer::RenderTorch(sf::RenderWindow& window, const sf::Vector2f& position, float animTime) {
    sf::Texture* texture = Resources::GetTexture("tileset");
    if (!texture) {
        return;
    }

    sf::Sprite sprite;
    sprite.setTexture(*texture);

    // Animate torch - cycle through frames
    int frameCount = 4; // Assume 4 animation frames
    int currentFrame = static_cast<int>(animTime * 8.0f) % frameCount; // 8 fps animation

    sf::IntRect torchRect = GetTilesetRect(TileType::TORCH);
    torchRect.left += currentFrame * torchRect.width;

    sprite.setTextureRect(torchRect);
    sprite.setPosition(position);

    float scaleX = tileSize / static_cast<float>(torchRect.width);
    float scaleY = tileSize / static_cast<float>(torchRect.height);
    sprite.setScale(scaleX, scaleY);

    window.draw(sprite);
}

sf::IntRect TileRenderer::GetTilesetRect(TileType type) const {
    // Find tile info for the given type
    for (const auto& tileInfo : tileInfos) {
        if (tileInfo.type == type) {
            // Assuming each tile is 32x32 pixels in the tileset
            int tilePixelSize = 32;
            return sf::IntRect(
                tileInfo.textureCoord.x * tilePixelSize,
                tileInfo.textureCoord.y * tilePixelSize,
                tilePixelSize,
                tilePixelSize
            );
        }
    }

    // Default rectangle if tile type not found
    return sf::IntRect(0, 0, 32, 32);
}

sf::Vector2f TileRenderer::GetWorldPosition(int gridX, int gridY, int gridWidth, int gridHeight) const {
    // Convert grid coordinates to world coordinates
    // Center the grid around origin
    float worldX = (static_cast<float>(gridX) - static_cast<float>(gridWidth) * 0.5f) * tileSize;
    float worldY = (static_cast<float>(gridY) - static_cast<float>(gridHeight) * 0.5f) * tileSize;

    return sf::Vector2f(worldX, worldY);
}