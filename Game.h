// Game.h - Declaraciones del sistema principal del juego
#pragma once

#include "Player.h"
#include "Camera.h"
#include "LevelEditor.h"
#include "Renderer.h"
#include <vector>
#include <fstream>
#include <iostream>
#include "Map.h"
#include "MapRenderer.h"
#include "Resources.h"
#include "SpriteLoader.h"
#include "Physics.h"
#include "Types.h"

// Forward declarations
class Camera;
class Player;
class LevelEditor;

// Global variables that need to be declared
extern bool showLevelEditor;
extern Camera camera;
extern Player player;
extern LevelEditor levelEditor;

// Game initialization and main functions
void init(sf::RenderWindow& window);
void HandleInput(sf::Event& event, sf::RenderWindow& window);
void Update(float deltaTime);
void Render(Renderer& renderer);
void RenderUI(sf::RenderWindow& window);

// Game state functions
void InitializeTileInfos();
void RenderTorch(sf::RenderWindow& window, const sf::Vector2f& position, float animTime);
void GetTilesIndex();
void resetAnimationTimer();
void climb();
void update();

// Camera class definition
class Camera {
private:
    sf::Vector2f position;
    sf::Vector2f targetPosition;
    float smoothing;
    sf::Vector2f bounds;

public:
    explicit Camera(float smoothingFactor = 0.1f)
        : position(0.0f, 0.0f), targetPosition(0.0f, 0.0f),
        smoothing(smoothingFactor), bounds(0.0f, 0.0f) {
    }

    void setTarget(const sf::Vector2f& target) {
        targetPosition = target;
    }

    void update(float deltaTime) {
        // Smooth camera movement
        sf::Vector2f diff = targetPosition - position;
        position += diff * smoothing;
    }

    sf::View getView(const sf::Vector2f& viewSize) const {
        sf::View view;
        view.setCenter(position);
        view.setSize(viewSize);
        return view;
    }

    sf::Vector2f getPosition() const {
        return position;
    }

    void setPosition(const sf::Vector2f& pos) {
        position = pos;
        targetPosition = pos;
    }
};