#pragma once
class Camera
{
public:
	Camera(float zoomlevel = 5.0f);
	sf::View getView(sf::Vector2f windowSize) const;

	float zoomLevel;
	sf::Vector2f position;
};

