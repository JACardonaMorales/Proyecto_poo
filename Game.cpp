#include "stdafx.h"
#include "Game.h"
#include "Resources.h"
#include "TileRenderer.h"
#include <iostream>

// Global variables
bool showLevelEditor = false;
Camera camera;
Player player;
LevelEditor levelEditor;
TileRenderer tileRenderer;
float gameTime = 0.0f;

void init(sf::RenderWindow& window) {
    std::cout << "Initializing game..." << std::endl;

    // Load resources
    if (!Resources::LoadTexture("tileset", "assets/tileset.png")) {
        std::cout << "Warning: Could not load tileset.png" << std::endl;
    }

    if (!Resources::LoadTexture("player", "assets/player.png")) {
        std::cout << "Warning: Could not load player.png" << std::endl;
    }

    // Initialize tile renderer
    tileRenderer.InitializeTileInfos();

    // Initialize camera
    camera.setPosition(sf::Vector2f(0.0f, 0.0f));

    std::cout << "Game initialized successfully!" << std::endl;
}

void HandleInput(sf::Event& event, sf::RenderWindow& window) {
    if (event.type == sf::Event::KeyPressed) {
        switch (event.key.code) {
        case sf::Keyboard::Tab:
            showLevelEditor = !showLevelEditor;
            std::cout << "Level Editor: " << (showLevelEditor ? "ON" : "OFF") << std::endl;
            break;

        case sf::Keyboard::F5:
            std::cout << "Quick save (not implemented)" << std::endl;
            break;

        case sf::Keyboard::F9:
            std::cout << "Quick load (not implemented)" << std::endl;
            break;

        default:
            break;
        }

        if (showLevelEditor) {
            // Handle level editor input
            HandleLevelEditorInput(event, window);
        }
        else {
            // Handle player input
            HandlePlayerInput(event);
        }
    }

    if (event.type == sf::Event::MouseButtonPressed) {
        if (showLevelEditor) {
            HandleLevelEditorInput(event, window);
        }
    }
}

void HandlePlayerInput(sf::Event& event) {
    // Handle continuous input for player movement
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A) || sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
        player.moveLeft();
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D) || sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
        player.moveRight();
    }
    if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Space) {
        player.jump();
    }
}

void HandleLevelEditorInput(sf::Event& event, sf::RenderWindow& window) {
    // Handle level editor specific input
    if (event.type == sf::Event::KeyPressed) {
        switch (event.key.code) {
        case sf::Keyboard::Num1:
            levelEditor.setSelectedTile(TileType::WALL);
            break;
        case sf::Keyboard::Num2:
            levelEditor.setSelectedTile(TileType::PLATFORM);
            break;
        case sf::Keyboard::Num3:
            levelEditor.setSelectedTile(TileType::SPIKES);
            break;
        case sf::Keyboard::Num4:
            levelEditor.setSelectedTile(TileType::TORCH);
            break;
        case sf::Keyboard::Num5:
            levelEditor.setSelectedTile(TileType::DOOR);
            break;
        case sf::Keyboard::F1:
            levelEditor.saveLevel("level.dat");
            break;
        case sf::Keyboard::F2:
            levelEditor.loadLevel("level.dat");
            break;
        case sf::Keyboard::G:
            levelEditor.toggleGrid();
            break;
        default:
            break;
        }
    }

    if (event.type == sf::Event::MouseButtonPressed) {
        sf::Vector2i mousePos = sf::Mouse::getPosition(window);
        sf::Vector2f worldPos = window.mapPixelToCoords(mousePos);

        if (event.mouseButton.button == sf::Mouse::Left) {
            levelEditor.placeTile(worldPos);
        }
        else if (event.mouseButton.button == sf::Mouse::Right) {
            levelEditor.eraseTile(worldPos);
        }
    }
}

void Update(float deltaTime) {
    gameTime += deltaTime;

    if (!showLevelEditor) {
        // Update player
        player.update(deltaTime);

        // Update camera to follow player
        camera.setTarget(player.getPosition());
    }

    // Update camera
    camera.update(deltaTime);
}

void Render(Renderer& renderer) {
    if (showLevelEditor) {
        levelEditor.render(renderer);
    }
    else {
        // Render game world
        RenderGameWorld(renderer);

        // Render player
        player.render(renderer);
    }
}

void RenderGameWorld(Renderer& renderer) {
    // Render tiles from level data
    // This is a basic implementation - you'll need to expand based on your level system
    sf::Texture* texture = Resources::GetTexture("tileset");
    if (texture) {
        // Example: render a simple ground
        for (int x = -10; x < 10; x++) {
            sf::Vector2f position(static_cast<float>(x) * 32.0f, 200.0f);
            renderer.Draw(*texture, position, sf::Vector2f(32.0f, 32.0f));
        }
    }
}

void RenderUI(sf::RenderWindow& window) {
    if (showLevelEditor) {
        levelEditor.renderUI(window);
    }

    // Render game UI (health, score, etc.)
    // Add your UI rendering code here
}

// Placeholder implementations for missing functions
void InitializeTileInfos() {
    tileRenderer.InitializeTileInfos();
}

void RenderTorch(sf::RenderWindow& window, const sf::Vector2f& position, float animTime) {
    tileRenderer.RenderTorch(window, position, animTime);
}

void GetTilesIndex() {
    // Implementation for getting tile indices
    // Add your logic here
}

void resetAnimationTimer() {
    // Reset animation timer logic
    gameTime = 0.0f;
}

void climb() {
    // Player climbing logic
    player.climb();
}

void update() {
    // General update function
    Update(1.0f / 60.0f); // Assume 60 FPS
}