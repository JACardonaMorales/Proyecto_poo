#pragma once
enum class TileType {
    EMPTY = 0,
    WALL = 1,
    PLATFORM = 2,
    SPIKES = 3,
    TORCH = 4,
    DOOR = 5,
    // Tileset blocks with different orientations
    BLOCK_TOP_LEFT = 10,
    BLOCK_TOP = 11,
    BLOCK_TOP_RIGHT = 12,
    BLOCK_LEFT = 13,
    BLOCK_CENTER = 14,
    BLOCK_RIGHT = 15,
    BLOCK_BOTTOM_LEFT = 16,
    BLOCK_BOTTOM = 17,
    BLOCK_BOTTOM_RIGHT = 18,
    // Shadow variations
    BLOCK_SHADOW_LEFT = 20,
    BLOCK_SHADOW_RIGHT = 21,
    BLOCK_SHADOW_BOTTOM = 22,
    BLOCK_SHADOW_CORNER = 23
};

struct Tile {
    TileType type;
    sf::Vector2f position;
    bool isAnimated;
    float animationTime;
    int currentFrame;

    Tile() : type(TileType::EMPTY), isAnimated(false), animationTime(0.0f), currentFrame(0) {}
};