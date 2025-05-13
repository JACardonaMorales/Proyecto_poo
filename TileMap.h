#pragma once

#include"Tile.h"

class TileMap
{
private:
	std::vector< std::vector<Tile*> > tiles;
	sf::Texture* tileSheet;
	unsigned tileSize;
	unsigned mapWidth;
	unsigned mapHeight;

public:
	TileMap();
	TileMap(unsigned width, unsigned height, sf::Texture* tile_sheet, unsigned tile_size);
	~TileMap();

	//Inline functions
	inline const unsigned& getTileSize() const { return this->tileSize; };
	inline const unsigned& getMapWidth() const { return this->mapWidth; };
	inline const unsigned& getMapHeight() const { return this->mapHeight; };

	//Functions
	void addTile(unsigned x, unsigned y, TileType type = TileType::SOLID, bool damaging = false);
	void removeTile(unsigned x, unsigned y);

	// Función para detectar colisiones con tiles
	bool checkCollision(const sf::FloatRect& bounds, sf::FloatRect& intersection, TileType& collisionType);

	// Función para cargar un nivel desde un archivo
	bool loadFromFile(const std::string& file_path);

	// Función para salvar el nivel actual a un archivo
	bool saveToFile(const std::string& file_path);

	void update();
	void render(sf::RenderTarget& target);
};

