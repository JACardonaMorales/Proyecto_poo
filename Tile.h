#pragma once

enum TILE_TYPES {
    EMPTY = 0,
    SOLID,
    PLATFORM,
    SPIKE,
    EXIT
};

class Tile
{
private:
    sf::Sprite sprite;
    sf::Texture textureSheet;
    sf::RectangleShape shape;
    sf::IntRect textureRect;
    bool damaging;
    short type;
    bool isCollider;

public:
    Tile(float x, float y, float gridSize, sf::Texture& texture, sf::IntRect textureRect, short type, bool isCollider);

    // Accessors
    const short& getType() const;
    const bool& getIsCollider() const;
    const sf::FloatRect getGlobalBounds() const;
    const sf::Vector2f getPosition() const;

    // Functions
	void update();

    void render(sf::RenderTarget& target);
};