#include "stdafx.h"
#include "Map.h"
#include "Resources.h"
#include "LevelEditor.h"
#include "Game.h"

Map::Map(float cellSize) : cellSize(cellSize)
{
    CreateDefaultTextures();
    CreatePlaceholderTextures();
}

void Map::createBoard(size_t width, size_t height)
{
    // Validar parámetros
    if (width == 0 || height == 0) {
        width = std::max((size_t)1, width);
        height = std::max((size_t)1, height);
    }

    grid.clear();
    grid.resize(height, std::vector<int>(width, 0));
}

sf::Vector2f Map::GetWorldPosition(int gridX, int gridY) const {
    float worldX = gridX * cellSize - (grid[0].size() * cellSize) / 2.0f + cellSize / 2.0f;
    float worldY = gridY * cellSize - (grid.size() * cellSize) / 2.0f + cellSize / 2.0f;
    return sf::Vector2f(worldX, worldY);
}

sf::Texture* Map::GetTileTexture(TileType type) {
    switch (type) {
    case TileType::WALL: return Resources::GetTexture("wall");
    case TileType::PLATFORM: return Resources::GetTexture("platform");
    case TileType::SPIKES: return Resources::GetTexture("spikes");
    case TileType::DOOR: return Resources::GetTexture("door");
    default: return nullptr;
    }
}
void Map::CreateDefaultTextures()
{
    // Load actual textures usando el sistema seguro
    Resources::LoadTexture("spikes", "assets/sprites/spike.png");
    Resources::LoadTexture("torch", "assets/sprites/torch.png");
    Resources::LoadTexture("tileset", "assets/sprites/tileset.png");
    Resources::LoadTexture("door", "assets/sprites/door.png");
}

void Map::CreatePlaceholderTextures()
{
    // Create placeholder textures for testing

    // Wall texture
    sf::Texture wallTexture;
    sf::Image wallImage;
    wallImage.create(32, 32, sf::Color(100, 100, 100));
    wallTexture.loadFromImage(wallImage);
    Resources::AddTexture("wall", wallTexture);

    // Platform texture (placeholder - replace with actual sprite later)
    sf::Texture platformTexture;
    sf::Image platformImage;
    platformImage.create(32, 8, sf::Color(139, 69, 19)); // Brown color
    // Add some detail to make it look like wood
    for (int x = 0; x < 32; x += 4) {
        for (int y = 0; y < 8; ++y) {
            if (x < 32) platformImage.setPixel(x, y, sf::Color(160, 82, 45));
        }
    }
    platformTexture.loadFromImage(platformImage);
    Resources::AddTexture("platform", platformTexture);
}

void Map::Draw(Renderer& renderer)
{
    // Verificar que el grid no esté vacío
    if (grid.empty() || grid[0].empty()) {
        return; // No hay nada que dibujar
    }

    static sf::Clock animationClock;
    float animationTime = animationClock.getElapsedTime().asSeconds();

    for (size_t y = 0; y < grid.size(); ++y) {
        for (size_t x = 0; x < grid[y].size(); ++x) {
            if (grid[y][x] != 0) {
                sf::Vector2f position(
                    x * cellSize - (grid[0].size() * cellSize) / 2.0f + cellSize / 2.0f,
                    y * cellSize - (grid.size() * cellSize) / 2.0f + cellSize / 2.0f
                );
                sf::Vector2f size(cellSize, cellSize);

                TileType tileType = static_cast<TileType>(grid[y][x]);

                // Special handling for animated torch
                if (tileType == TileType::TORCH) {
                    DrawAnimatedTorch(renderer, position, size, animationTime);
                }
                else {
                    DrawTilesetTile(renderer, position, size, tileType);
                }
            }
        }
    }
}

void Map::DrawAnimatedTorch(Renderer& renderer, const sf::Vector2f& position, const sf::Vector2f& size, float animationTime)
{
    sf::Texture* texture = Resources::GetTexture("torch");
    if (texture) {
        sf::Sprite sprite;
        sprite.setTexture(*texture);

        // Calculate animation frame (assuming 8 frames horizontally)
        int frameWidth = texture->getSize().x / 8;
        int frameHeight = texture->getSize().y;
        int currentFrame = (int)(animationTime * 8.0f) % 8; // 8 frames animation

        sf::IntRect frameRect(currentFrame * frameWidth, 0, frameWidth, frameHeight);
        sprite.setTextureRect(frameRect);

        sprite.setOrigin(frameWidth / 2.0f, frameHeight / 2.0f);
        sprite.setPosition(position);

        // Scale to fit tile size
        float scaleX = size.x / frameWidth;
        float scaleY = size.y / frameHeight;
        sprite.setScale(scaleX, scaleY);

        // Para texturas con rects específicos, necesitarás extender tu Renderer
        // Por ahora usamos el método básico
        renderer.Draw(*texture, position, size);
    }
}

void Map::DrawTilesetTile(Renderer& renderer, const sf::Vector2f& position, const sf::Vector2f& size, TileType tileType)
{
    std::string textureName;
    sf::IntRect textureRect;

    switch (tileType) {
    case TileType::WALL:
        textureName = "wall";
        break;

    case TileType::PLATFORM:
        textureName = "platform";
        break;

    case TileType::SPIKES:
        textureName = "spikes";
        break;

    case TileType::DOOR:
        textureName = "door";
        break;

        // Tileset tiles - assuming 16x16 tiles in the tileset
    case TileType::BLOCK_TOP_LEFT:
        textureName = "tileset";
        textureRect = sf::IntRect(0, 0, 32, 32);
        break;

    case TileType::BLOCK_TOP:
        textureName = "tileset";
        textureRect = sf::IntRect(32, 0, 32, 32);
        break;

    case TileType::BLOCK_TOP_RIGHT:
        textureName = "tileset";
        textureRect = sf::IntRect(64, 0, 32, 32);
        break;

    case TileType::BLOCK_LEFT:
        textureName = "tileset";
        textureRect = sf::IntRect(0, 32, 32, 32);
        break;

    case TileType::BLOCK_CENTER:
        textureName = "tileset";
        textureRect = sf::IntRect(32, 32, 32, 32);
        break;

    case TileType::BLOCK_RIGHT:
        textureName = "tileset";
        textureRect = sf::IntRect(64, 32, 32, 32);
        break;

    case TileType::BLOCK_BOTTOM_LEFT:
        textureName = "tileset";
        textureRect = sf::IntRect(0, 64, 32, 32);
        break;

    case TileType::BLOCK_BOTTOM:
        textureName = "tileset";
        textureRect = sf::IntRect(32, 64, 32, 32);
        break;

    case TileType::BLOCK_BOTTOM_RIGHT:
        textureName = "tileset";
        textureRect = sf::IntRect(64, 64, 32, 32);
        break;

    case TileType::BLOCK_SHADOW_LEFT:
        textureName = "tileset";
        textureRect = sf::IntRect(96, 0, 32, 32);
        break;

    case TileType::BLOCK_SHADOW_RIGHT:
        textureName = "tileset";
        textureRect = sf::IntRect(96, 32, 32, 32);
        break;

    case TileType::BLOCK_SHADOW_BOTTOM:
        textureName = "tileset";
        textureRect = sf::IntRect(96, 64, 32, 32);
        break;

    case TileType::BLOCK_SHADOW_CORNER:
        textureName = "tileset";
        textureRect = sf::IntRect(96, 96, 32, 32);
        break;

    default:
        return; // Don't draw unknown tiles
    }

    sf::Texture* texture = Resources::GetTexture(textureName);
    if (texture) {
        renderer.Draw(*texture, position, size);
    }
}

void Map::InitFromImage(const sf::Image& image)
{
    createBoard(image.getSize().x, image.getSize().y);

    for (unsigned int y = 0; y < image.getSize().y; ++y) {
        for (unsigned int x = 0; x < image.getSize().x; ++x) {
            sf::Color pixel = image.getPixel(x, y);

            // Map colors to tile types
            if (pixel == sf::Color::Black) {
                grid[y][x] = static_cast<int>(TileType::EMPTY);
            }
            else if (pixel == sf::Color::White) {
                grid[y][x] = static_cast<int>(TileType::WALL);
            }
            else if (pixel == sf::Color::Red) {
                grid[y][x] = static_cast<int>(TileType::PLATFORM);
            }
            else if (pixel == sf::Color::Magenta) { // Cambié esto para que sea diferente de Red
                grid[y][x] = static_cast<int>(TileType::SPIKES);
            }
            else if (pixel == sf::Color::Yellow) {
                grid[y][x] = static_cast<int>(TileType::TORCH);
            }
            else if (pixel == sf::Color::Blue) {
                grid[y][x] = static_cast<int>(TileType::DOOR);
            }
            else if (pixel == sf::Color::Green) {
                // Default to center block for green pixels
                grid[y][x] = static_cast<int>(TileType::BLOCK_CENTER);
            }
            else {
                grid[y][x] = static_cast<int>(TileType::EMPTY);
            }
        }
    }
}