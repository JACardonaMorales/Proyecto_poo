// Camera.h
#pragma once
#include "Player.h"

class Camera {
private:
    sf::View view;
    Player* target;
    sf::Vector2f bounds_min;
    sf::Vector2f bounds_max;
    float smoothing;

public:
    Camera();

    void setTarget(Player* player);

    void setBounds(float minX, float minY, float maxX, float maxY);

    void update();

    sf::View getView(const sf::Vector2f& windowSize);
};
