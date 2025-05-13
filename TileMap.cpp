#include "stdafx.h"
#include "TileMap.h"
#include <fstream>

TileMap::TileMap()
{
    this->tileSheet = NULL;
    this->tileSize = 0;
    this->mapWidth = 0;
    this->mapHeight = 0;
}

TileMap::TileMap(unsigned width, unsigned height, sf::Texture* tile_sheet, unsigned tile_size)
{
    this->tileSheet = tile_sheet;
    this->tileSize = tile_size;
    this->mapWidth = width;
    this->mapHeight = height;

    this->tiles.resize(width);
    for (int i = 0; i < this->tiles.size(); i++)
    {
        this->tiles[i].resize(height, nullptr);
    }
}

TileMap::~TileMap()
{
    for (int i = 0; i < this->tiles.size(); i++)
    {
        for (int k = 0; k < this->tiles[i].size(); k++)
        {
            delete this->tiles[i][k];
            this->tiles[i][k] = nullptr;
        }
    }
}

void TileMap::addTile(unsigned x, unsigned y, TileType type, bool damaging)
{
    if (x < this->tiles.size() && x >= 0)
    {
        if (y < this->tiles[x].size() && y >= 0)
        {
            // Eliminar el tile existente si hay uno
            if (this->tiles[x][y] != nullptr)
            {
                delete this->tiles[x][y];
                this->tiles[x][y] = nullptr;
            }

            // Crear un nuevo tile con el tipo especificado
            sf::IntRect textureRect;

            // Establecer la textura según el tipo de tile
            switch (type)
            {
            case TileType::SOLID:
                textureRect = sf::IntRect(0, 0, this->tileSize, this->tileSize);
                break;
            case TileType::PLATFORM:
                textureRect = sf::IntRect(this->tileSize, 0, this->tileSize, this->tileSize);
                break;
            case TileType::LADDER:
                textureRect = sf::IntRect(this->tileSize * 2, 0, this->tileSize, this->tileSize);
                break;
            case TileType::SPIKE:
                textureRect = sf::IntRect(this->tileSize * 3, 0, this->tileSize, this->tileSize);
                break;
            case TileType::DECORATION:
                textureRect = sf::IntRect(this->tileSize * 4, 0, this->tileSize, this->tileSize);
                break;
            default:
                textureRect = sf::IntRect(0, 0, this->tileSize, this->tileSize);
                break;
            }

            this->tiles[x][y] = new Tile(x, y, this->tileSize, this->tileSheet, textureRect, type, damaging);
        }
    }
}

void TileMap::removeTile(unsigned x, unsigned y)
{
    if (x < this->tiles.size())
    {
        if (y < this->tiles[x].size())
        {
            if (this->tiles[x][y] != nullptr)
            {
                delete this->tiles[x][y];
                this->tiles[x][y] = nullptr;
            }
        }
    }
}

bool TileMap::checkCollision(const sf::FloatRect& bounds, sf::FloatRect& intersection, TileType& collisionType)
{
    // Calcular las coordenadas de rejilla potenciales para la colisión (expandidas para incluir tiles adyacentes)
    int startX = std::max(0, (int)(bounds.left / this->tileSize) - 1);
    int endX = std::min((int)this->mapWidth - 1, (int)((bounds.left + bounds.width) / this->tileSize) + 1);
    int startY = std::max(0, (int)(bounds.top / this->tileSize) - 1);
    int endY = std::min((int)this->mapHeight - 1, (int)((bounds.top + bounds.height) / this->tileSize) + 1);

    bool collision = false;
    float minOverlap = std::numeric_limits<float>::max();

    // Revisar todos los tiles potenciales para colisión
    for (int x = startX; x <= endX; x++)
    {
        for (int y = startY; y <= endY; y++)
        {
            if (this->tiles[x][y] != nullptr)
            {
                // Obtener el tipo de tile
                TileType tileType = this->tiles[x][y]->getType();

                // Ignorar tiles decorativos (no tienen colisión)
                if (tileType == TileType::DECORATION)
                    continue;

                sf::FloatRect tileBox = this->tiles[x][y]->getGlobalBounds();
                sf::FloatRect overlap;

                // Comprobar si hay intersección
                if (tileBox.intersects(bounds, overlap))
                {
                    // Para plataformas, solo colisionar si el jugador está cayendo y su pie está por encima de la plataforma
                    if (tileType == TileType::PLATFORM)
                    {
                        // Si el pie del jugador está por encima de la plataforma, hay colisión
                        if (bounds.top + bounds.height <= tileBox.top + 10.0f)
                        {
                            collision = true;
                            collisionType = tileType;
                            intersection = overlap;
                            return true;
                        }
                    }
                    // Para otros tipos, colisionar normalmente
                    else
                    {
                        // Calcular el "tamaño" de la colisión (área de intersección)
                        float overlapSize = overlap.width * overlap.height;

                        if (overlapSize < minOverlap)
                        {
                            minOverlap = overlapSize;
                            collision = true;
                            collisionType = tileType;
                            intersection = overlap;
                        }
                    }
                }
            }
        }
    }

    return collision;
}

bool TileMap::loadFromFile(const std::string& file_path)
{
    std::ifstream file(file_path);
    if (!file.is_open())
    {
        std::cout << "ERROR::TILEMAP::Could not open file: " << file_path << std::endl;
        return false;
    }

    // Limpiar los tiles existentes
    for (int i = 0; i < this->tiles.size(); i++)
    {
        for (int k = 0; k < this->tiles[i].size(); k++)
        {
            delete this->tiles[i][k];
            this->tiles[i][k] = nullptr;
        }
    }

    // Leer las dimensiones del mapa
    unsigned int width, height;
    file >> width >> height;

    // Redimensionar el arreglo de tiles
    this->mapWidth = width;
    this->mapHeight = height;
    this->tiles.resize(width);
    for (int i = 0; i < this->tiles.size(); i++)
    {
        this->tiles[i].resize(height, nullptr);
    }

    // Leer los datos de los tiles
    int tileValue;
    for (unsigned int y = 0; y < height; y++)
    {
        for (unsigned int x = 0; x < width; x++)
        {
            file >> tileValue;

            if (tileValue != -1) // -1 representa espacio vacío
            {
                TileType type = static_cast<TileType>(tileValue);
                bool damaging = (type == TileType::SPIKE); // Pinchos son dañinos

                this->addTile(x, y, type, damaging);
            }
        }
    }

    file.close();
    return true;
}

bool TileMap::saveToFile(const std::string& file_path)
{
    std::ofstream file(file_path);
    if (!file.is_open())
    {
        std::cout << "ERROR::TILEMAP::Could not open file for writing: " << file_path << std::endl;
        return false;
    }

    // Escribir las dimensiones del mapa
    file << this->mapWidth << " " << this->mapHeight << std::endl;

    // Escribir los datos de los tiles
    for (unsigned int y = 0; y < this->mapHeight; y++)
    {
        for (unsigned int x = 0; x < this->mapWidth; x++)
        {
            if (this->tiles[x][y] != nullptr)
            {
                file << static_cast<int>(this->tiles[x][y]->getType()) << " ";
            }
            else
            {
                file << -1 << " "; // -1 representa espacio vacío
            }
        }
        file << std::endl;
    }

    file.close();
    return true;
}

void TileMap::update()
{
    for (int i = 0; i < this->tiles.size(); i++)
    {
        for (int k = 0; k < this->tiles[i].size(); k++)
        {
            if (this->tiles[i][k] != nullptr)
                this->tiles[i][k]->update();
        }
    }
}

void TileMap::render(sf::RenderTarget& target)
{
    // Obtener los límites de lo que se está viendo en la pantalla
    sf::View view = target.getView();
    sf::FloatRect viewBounds(
        view.getCenter().x - view.getSize().x / 2,
        view.getCenter().y - view.getSize().y / 2,
        view.getSize().x,
        view.getSize().y
    );

    // Calcular cuáles tiles estarían visibles
    int startX = std::max(0, static_cast<int>(viewBounds.left / this->tileSize) - 1);
    int endX = std::min(static_cast<int>(this->mapWidth) - 1, static_cast<int>((viewBounds.left + viewBounds.width) / this->tileSize) + 1);
    int startY = std::max(0, static_cast<int>(viewBounds.top / this->tileSize) - 1);
    int endY = std::min(static_cast<int>(this->mapHeight) - 1, static_cast<int>((viewBounds.top + viewBounds.height) / this->tileSize) + 1);

    // Renderizar solo los tiles visibles
    for (int x = startX; x <= endX; x++)
    {
        for (int y = startY; y <= endY; y++)
        {
            if (this->tiles[x][y] != nullptr)
                this->tiles[x][y]->render(target);
        }
    }
}