#include "stdafx.h"
#include "Game.h"
#include "SpriteLoader.h"

// Global objects
Camera camera(5.0f);
Player player;
Map gameMap(32.0f);
LevelEditor levelEditor;
bool showLevelEditor = false;

void init(const sf::Window& window) {
    // Initialize sprite loader and load assets
    SpriteLoader& loader = SpriteLoader::getInstance();

    // Load player texture
    if (!Resources::LoadTexture("Ray", "assets/Ray/textures/RaySprite.png")) {
        std::cout << "Warning: Could not load player texture" << std::endl;
    }

    // Load platform sprite
    loader.LoadPlatformSprite("assets/sprites/platform.png");

    // Load tileset
    loader.LoadTileset("assets/sprites/tileset.png", 32, 32);

    // Initialize level editor
    levelEditor.Initialize();

    // Create default map
    gameMap.createBoard(50, 30);

    // Set initial camera position
    camera.position = sf::Vector2f(0.0f, 0.0f);

    std::cout << "Game initialized successfully!" << std::endl;
}

void HandleInput(const sf::Event& event, const sf::RenderWindow& window) {
    if (event.type == sf::Event::KeyPressed) {
        // Toggle level editor
        if (event.key.code == sf::Keyboard::Tab) {
            showLevelEditor = !showLevelEditor;
            levelEditor.isActive = showLevelEditor;
        }

        // Quick save/load
        if (event.key.code == sf::Keyboard::F5) {
            levelEditor.SaveLevel("quicksave.txt");
            std::cout << "Quick saved!" << std::endl;
        }
        if (event.key.code == sf::Keyboard::F9) {
            levelEditor.LoadLevel("quicksave.txt");
            SyncMapWithEditor();
            std::cout << "Quick loaded!" << std::endl;
        }
    }

    // Handle level editor input
    if (showLevelEditor) {
        levelEditor.HandleInput(event, window);
    }
}

void Update(float deltaTime) {
    if (showLevelEditor) {
        levelEditor.Update(deltaTime);
    }
    else {
        // Update player
        player.update();

        // Handle physics
        Physics::HandlePlayerCollision(player, gameMap);

        // Update camera to follow player
        camera.position = player.getPosition();
    }
}

void Render(Renderer& renderer) {
    if (showLevelEditor) {
        // Level editor handles its own rendering
    }
    else {
        // Render map
        gameMap.Draw(renderer);

        // Render player
        player.render(renderer.GetTarget());
    }
}

void RenderUI(sf::RenderWindow& window) {
    if (showLevelEditor) {
        levelEditor.Render(window);
    }

    // Add game UI here if needed
}

void SyncMapWithEditor() {
    const auto& editorGrid = levelEditor.GetGrid();
    gameMap.grid.clear();
    gameMap.grid.resize(editorGrid.size());

    for (size_t y = 0; y < editorGrid.size(); ++y) {
        gameMap.grid[y].resize(editorGrid[y].size());
        for (size_t x = 0; x < editorGrid[y].size(); ++x) {
            gameMap.grid[y][x] = static_cast<int>(editorGrid[y][x]);
        }
    }
}