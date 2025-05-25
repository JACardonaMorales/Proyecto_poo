#include "stdafx.h"
#include "Tile.h"

Tile::Tile(float x, float y, float gridSize, sf::Texture& texture, sf::IntRect textureRect, short type, bool isCollider): textureRect(textureRect), type(type), isCollider(isCollider)
{

	this->shape.setSize(sf::Vector2f(gridSize, gridSize));
	this->shape.setPosition(x, y);
	this->shape.setTexture(&texture);
	this->shape.setTextureRect(textureRect);
	this->damaging = false;
	if (this->type == TILE_TYPES::SPIKE)
	{
		this->damaging = true;
	}
	else
	{
		this->damaging = false;
	}
}

const short& Tile::getType() const
{
	return this->type;
}

const bool& Tile::getIsCollider() const
{
	return this->isCollider;
}

const sf::FloatRect Tile::getGlobalBounds() const
{
	return this->shape.getGlobalBounds();
}

const sf::Vector2f Tile::getPosition() const
{
	return this->shape.getPosition();
}

void Tile::update()
{
	// Update logic can be added here if needed
}

void Tile::render(sf::RenderTarget& target)
{
	target.draw(this->shape);
}
