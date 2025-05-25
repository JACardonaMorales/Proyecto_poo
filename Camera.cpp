#include "stdafx.h"
#include "Camera.h"

Camera::Camera(float zoomlevel)
	: zoomLevel(zoomlevel)
{

}

sf::View Camera::getView(sf::Vector2f windowSize) const
{
	float aspect = (float)windowSize.x / (float)windowSize.y;
	sf::Vector2f size;
	if (aspect < 1.0f)
	
		size = sf::Vector2f(zoomLevel, zoomLevel / aspect);
	else
		size = sf::Vector2f(zoomLevel * aspect, zoomLevel);

	return sf::View(position, size);
}
