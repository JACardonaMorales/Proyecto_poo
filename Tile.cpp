#include "stdafx.h"
#include "Tile.h"

Tile::Tile(const unsigned& grid_pos_x, const unsigned& grid_pos_y, const unsigned& tile_size,
	sf::Texture* texture_sheet, sf::IntRect texture_rect, TileType type, bool damaging)
	: type(type), damaging(damaging)
{
	this->sprite.setTexture(*texture_sheet);
	this->sprite.setTextureRect(texture_rect);
	this->sprite.setPosition(float(grid_pos_x * tile_size), float(grid_pos_y * tile_size));
}

const sf::FloatRect Tile::getGlobalBounds() const
{
	return this->sprite.getGlobalBounds();
}

const TileType& Tile::getType() const
{
	return this->type;
}

const sf::Vector2f& Tile::getPosition() const
{
	return this->sprite.getPosition();
}

const bool Tile::isDamaging() const
{
	return this->damaging;
}

void Tile::update()
{
	// Se puede agregar animación u otros comportamientos aquí
}

void Tile::render(sf::RenderTarget& target)
{
	target.draw(this->sprite);
}