#include "stdafx.h"
#include "Camera.h"
#include "Game.h"

Camera::Camera(): target(nullptr), smoothing(0.1f)
{
	view.setSize(1200.0f, 800.0f);
	view.setCenter(0.0f, 0.0f);
}

void Camera::setTarget(Player* player)
{ target = player; }

void Camera::setBounds(float minX, float minY, float maxX, float maxY)
{
	bounds_min = sf::Vector2f(minX, minY);
	bounds_max = sf::Vector2f(maxX, maxY);
}

void Camera::update()
{
	if (target) {
		sf::Vector2f targetPos = target->getPosition();
		sf::Vector2f currentCenter = view.getCenter();

		// Suavizar el movimiento de la cámara
		sf::Vector2f newCenter = currentCenter + (targetPos - currentCenter) * smoothing;

		// Aplicar límites
		if (newCenter.x < bounds_min.x) newCenter.x = bounds_min.x;
		if (newCenter.x > bounds_max.x) newCenter.x = bounds_max.x;
		if (newCenter.y < bounds_min.y) newCenter.y = bounds_min.y;
		if (newCenter.y > bounds_max.y) newCenter.y = bounds_max.y;

		view.setCenter(newCenter);
	}
}

sf::View Camera::getView(const sf::Vector2f& windowSize)
{
	view.setSize(windowSize);
	return view;
}
