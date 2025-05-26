#include "stdafx.h"
#include "Game.h"
#include "Map.h"

Map::Map(float cellSize) : cellSize(cellSize)
{
    CreateDefaultTextures();
}

void Map::createBoard(size_t width, size_t height)
{
    grid.clear();
    grid.resize(height, std::vector<int>(width, 0));
}

void Map::CreateDefaultTextures()
{
    // Cargar las texturas de los sprites
    Resources::textures["spike"].loadFromFile("assets/sprites/spike.png");
    Resources::textures["torch"].loadFromFile("assets/sprites/torch_spritesheet.png"); // Spritesheet de antorcha
    Resources::textures["tileset"].loadFromFile("assets/sprites/tileset.png");
    Resources::textures["door"].loadFromFile("assets/sprites/door.png");
    Resources::textures["platform"].loadFromFile("assets/sprites/platform.png");
    Resources::textures["solid_block"].loadFromFile("assets/sprites/solid_block.png");

    // Si no se pueden cargar las texturas, crear texturas de placeholder
    if (Resources::textures["spike"].getSize().x == 0)
    {
        CreatePlaceholderTextures();
    }
}

void Map::CreatePlaceholderTextures()
{
    // Crear texturas de placeholder si no se encuentran los archivos
    sf::Image placeholder;

    // Placeholder para spike (rojo)
    placeholder.create(32, 32, sf::Color::Red);
    Resources::textures["spike"].loadFromImage(placeholder);

    // Placeholder para torch (amarillo)
    placeholder.create(32, 32, sf::Color::Yellow);
    Resources::textures["torch"].loadFromImage(placeholder);

    // Placeholder para tileset (gris)
    placeholder.create(32, 32, sf::Color(128, 128, 128));
    Resources::textures["tileset"].loadFromImage(placeholder);

    // Placeholder para door (marrón)
    placeholder.create(32, 32, sf::Color(139, 69, 19));
    Resources::textures["door"].loadFromImage(placeholder);

    // Placeholder para platform (verde)
    placeholder.create(32, 32, sf::Color::Green);
    Resources::textures["platform"].loadFromImage(placeholder);

    // Placeholder para solid block (azul)
    placeholder.create(32, 32, sf::Color::Blue);
    Resources::textures["solid_block"].loadFromImage(placeholder);
}

void Map::Draw(Renderer& renderer)
{
    static sf::Clock animationClock; // Para animaciones
    float animationTime = animationClock.getElapsedTime().asSeconds();

    for (size_t y = 0; y < grid.size(); ++y)
    {
        for (size_t x = 0; x < grid[y].size(); ++x)
        {
            TileType tileType = static_cast<TileType>(grid[y][x]);

            if (tileType == TileType::EMPTY) continue;

            sf::Vector2f position(x * cellSize + cellSize / 2, y * cellSize + cellSize / 2);
            sf::Vector2f size(cellSize, cellSize);

            switch (tileType)
            {
            case TileType::SOLID_BLOCK:
                renderer.Draw(Resources::textures["solid_block"], position, size);
                break;

            case TileType::SPIKE:
                renderer.Draw(Resources::textures["spike"], position, size);
                break;

            case TileType::DOOR:
                renderer.Draw(Resources::textures["door"], position, size);
                break;

            case TileType::TORCH:
                DrawAnimatedTorch(renderer, position, size, animationTime);
                break;

            case TileType::PLATFORM:
                renderer.Draw(Resources::textures["platform"], position, size);
                break;

                // Tiles del tileset
            case TileType::BLOCK_TOP_LEFT:
            case TileType::BLOCK_TOP:
            case TileType::BLOCK_TOP_RIGHT:
            case TileType::BLOCK_LEFT:
            case TileType::BLOCK_CENTER:
            case TileType::BLOCK_RIGHT:
            case TileType::BLOCK_BOTTOM_LEFT:
            case TileType::BLOCK_BOTTOM:
            case TileType::BLOCK_BOTTOM_RIGHT:
            case TileType::BLOCK_SHADOW_TOP:
            case TileType::BLOCK_SHADOW_LEFT:
            case TileType::BLOCK_SHADOW_RIGHT:
            case TileType::BLOCK_SHADOW_BOTTOM:
                DrawTilesetTile(renderer, position, size, tileType);
                break;
            }
        }
    }
}

void Map::DrawAnimatedTorch(Renderer& renderer, const sf::Vector2f& position, const sf::Vector2f& size, float animationTime)
{
    // Asumiendo que la antorcha tiene 8 frames de animación
    int frameCount = 8;
    float frameTime = 0.1f; // 10 frames por segundo
    int currentFrame = static_cast<int>(animationTime / frameTime) % frameCount;

    // Si tienes un spritesheet, aquí extraerías el frame correcto
    // Por ahora, usamos la textura completa
    renderer.Draw(Resources::textures["torch"], position, size);
}

void Map::DrawTilesetTile(Renderer& renderer, const sf::Vector2f& position, const sf::Vector2f& size, TileType tileType)
{
    // Mapear cada tipo de tile a su posición en el tileset
    // Asumiendo un tileset de 4x5 (basado en tu descripción)
    sf::IntRect sourceRect;
    int tileSize = 32; // Tamaño de cada tile en el tileset

    switch (tileType)
    {
    case TileType::BLOCK_TOP_LEFT:
        sourceRect = sf::IntRect(0, 0, tileSize, tileSize);
        break;
    case TileType::BLOCK_TOP:
        sourceRect = sf::IntRect(tileSize, 0, tileSize, tileSize);
        break;
    case TileType::BLOCK_TOP_RIGHT:
        sourceRect = sf::IntRect(tileSize * 2, 0, tileSize, tileSize);
        break;
    case TileType::BLOCK_LEFT:
        sourceRect = sf::IntRect(0, tileSize, tileSize, tileSize);
        break;
    case TileType::BLOCK_CENTER:
        sourceRect = sf::IntRect(tileSize, tileSize, tileSize, tileSize);
        break;
    case TileType::BLOCK_RIGHT:
        sourceRect = sf::IntRect(tileSize * 2, tileSize, tileSize, tileSize);
        break;
    case TileType::BLOCK_BOTTOM_LEFT:
        sourceRect = sf::IntRect(0, tileSize * 2, tileSize, tileSize);
        break;
    case TileType::BLOCK_BOTTOM:
        sourceRect = sf::IntRect(tileSize, tileSize * 2, tileSize, tileSize);
        break;
    case TileType::BLOCK_BOTTOM_RIGHT:
        sourceRect = sf::IntRect(tileSize * 2, tileSize * 2, tileSize, tileSize);
        break;
    case TileType::BLOCK_SHADOW_TOP:
        sourceRect = sf::IntRect(tileSize * 3, 0, tileSize, tileSize);
        break;
    case TileType::BLOCK_SHADOW_LEFT:
        sourceRect = sf::IntRect(tileSize * 3, tileSize, tileSize, tileSize);
        break;
    case TileType::BLOCK_SHADOW_RIGHT:
        sourceRect = sf::IntRect(tileSize * 3, tileSize * 2, tileSize, tileSize);
        break;
    case TileType::BLOCK_SHADOW_BOTTOM:
        sourceRect = sf::IntRect(tileSize * 3, tileSize * 3, tileSize, tileSize);
        break;
    }

    // Aquí necesitarías una función Draw que acepte un IntRect para el sprite
    // Por ahora, usamos la textura completa
    renderer.Draw(Resources::textures["tileset"], position, size);
}

void Map::InitFromImage(const sf::Image& image)
{
    if (image.getSize().x == 0 || image.getSize().y == 0) return;

    createBoard(image.getSize().x, image.getSize().y);

    for (unsigned int y = 0; y < image.getSize().y; ++y)
    {
        for (unsigned int x = 0; x < image.getSize().x; ++x)
        {
            sf::Color pixel = image.getPixel(x, y);

            // Mapear colores a tipos de tiles
            TileType tileType = TileType::EMPTY;

            if (pixel == sf::Color::Black)
                tileType = TileType::SOLID_BLOCK;
            else if (pixel == sf::Color::Red)
                tileType = TileType::SPIKE;
            else if (pixel == sf::Color::Yellow)
                tileType = TileType::TORCH;
            else if (pixel == sf::Color::Green)
                tileType = TileType::PLATFORM;
            else if (pixel == sf::Color(139, 69, 19)) // Marrón para puerta
                tileType = TileType::DOOR;

            grid[y][x] = static_cast<int>(tileType);
        }
    }
}