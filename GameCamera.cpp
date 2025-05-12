#include "stdafx.h"
#include "GameCamera.h"

GameCamera() {
    // Initialize camera with the game window dimensions
    camera.setSize(width, height);

    // Optional: Set default smooth camera movement
    smoothSpeed = 0.1f;

    // Optional: Set a small camera offset if desired
    cameraOffset = sf::Vector2f(0.f, 0.f);
}

// Update camera to follow player with smooth movement
void update(const sf::Vector2f& playerPosition) {
    // Calculate target position with optional offset
    targetPosition = playerPosition + cameraOffset;

    // Smooth camera lerp (linear interpolation)
    sf::Vector2f currentCenter = camera.getCenter();
    sf::Vector2f smoothedPosition = currentCenter +
        (targetPosition - currentCenter) * smoothSpeed;

    camera.setCenter(smoothedPosition);
}

// Getter for the camera view
sf::View& getView() {
    return camera;
}

// Set camera smooth speed (0.0 - 1.0)
void setSmoothSpeed(float speed) {
    smoothSpeed = std::max(0.0f, std::min(1.0f, speed));
}

// Set camera offset relative to player
void setCameraOffset(float offsetX, float offsetY) {
    cameraOffset = sf::Vector2f(offsetX, offsetY);
}
};