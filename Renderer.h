#pragma once
#include <SFML/Graphics.hpp>
class Renderer
{
public:
	Renderer(sf::RenderTarget& target);

	void Draw(const sf::Texture& texture, const sf::Vector2f& position, const sf::Vector2f& size);

	// Add a public accessor for the target
	sf::RenderTarget& GetTarget();
private:
	sf::Sprite sprite;
	sf::RenderTarget& target;
};



