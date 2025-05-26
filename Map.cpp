#include "stdafx.h"
#include "Map.h"
#include "Resources.h"

Map::Map(float cellSize) : cellSize(cellSize)
{
    // Constructor
}

void Map::createBoard(size_t width, size_t height)
{
    grid.clear();
    grid.resize(width, std::vector<int>(height, 0)); // 0 = EMPTY por defecto

    std::cout << "Map created with size: " << width << "x" << height << std::endl;
}

void Map::Draw(Renderer& renderer)
{
    for (size_t x = 0; x < grid.size(); ++x) {
        for (size_t y = 0; y < grid[x].size(); ++y) {
            int tileType = grid[x][y];

            if (tileType != 0) { // Solo dibujar tiles que no sean vacíos
                sf::Vector2f position(
                    x * cellSize + cellSize / 2.0f,
                    y * cellSize + cellSize / 2.0f
                );
                sf::Vector2f size(cellSize, cellSize);

                // Usar diferentes texturas basadas en el tipo de tile
                std::string textureName;
                switch (tileType) {
                case 1: // SOLID_BLOCK
                    textureName = "Tile";
                    break;
                case 2: // SPIKE
                    textureName = "Spike";
                    break;
                case 3: // DOOR
                    textureName = "Door";
                    break;
                default:
                    textureName = "Tile";
                    break;
                }

                // Si no tenemos texturas cargadas, crear texturas simples de colores
                if (Resources::textures.find(textureName) == Resources::textures.end()) {
                    CreateDefaultTextures();
                }

                if (Resources::textures.find(textureName) != Resources::textures.end()) {
                    renderer.Draw(Resources::textures[textureName], position, size);
                }
            }
        }
    }
}

void Map::InitFromImage(const sf::Image& image)
{
    unsigned int width = image.getSize().x;
    unsigned int height = image.getSize().y;

    createBoard(width, height);

    for (unsigned int x = 0; x < width; ++x) {
        for (unsigned int y = 0; y < height; ++y) {
            sf::Color pixel = image.getPixel(x, y);

            // Convertir colores a tipos de tiles
            if (pixel == sf::Color::Black) {
                grid[x][y] = 1; // SOLID_BLOCK
            }
            else if (pixel == sf::Color::Red) {
                grid[x][y] = 2; // SPIKE
            }
            else if (pixel == sf::Color::Blue) {
                grid[x][y] = 3; // DOOR
            }
            else {
                grid[x][y] = 0; // EMPTY
            }
        }
    }

    std::cout << "Map initialized from image: " << width << "x" << height << std::endl;
}

void Map::CreateDefaultTextures()
{
    // Crear texturas simples de colores si no tenemos archivos de imagen
    sf::Image solidImage;
    solidImage.create(32, 32, sf::Color(128, 128, 128)); // Gris para bloques sólidos
    Resources::textures["Tile"].loadFromImage(solidImage);

    sf::Image spikeImage;
    spikeImage.create(32, 32, sf::Color::Red); // Rojo para pinchos
    Resources::textures["Spike"].loadFromImage(spikeImage);

    sf::Image doorImage;
    doorImage.create(32, 32, sf::Color::Blue); // Azul para puertas
    Resources::textures["Door"].loadFromImage(doorImage);

    std::cout << "Default textures created" << std::endl;
}