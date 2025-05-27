#include "stdafx.h"
#include "Game.h"

#include "Resources.h"
#include <iostream>
#include <sstream>
#include <SFML/Graphics.hpp>

GameMode::GameMode()
    : player(nullptr), isInitialized(false), isPaused(false), showDebugInfo(false) {
    InitializeUI();
}

GameMode::~GameMode() {
    // Player is managed externally, don't delete here
}

void GameMode::InitializeUI() {
    // Try to load a default font
    if (!font.loadFromFile("assets/fonts/arial.ttf")) {
        // If no font file available, use default font
        std::cout << "Warning: Could not load font file, using default font" << std::endl;
    }

    debugText.setFont(font);
    debugText.setCharacterSize(16);
    debugText.setFillColor(sf::Color::White);
    debugText.setOutlineColor(sf::Color::Black);
    debugText.setOutlineThickness(1.0f);
    debugText.setPosition(10.0f, 10.0f);
}

bool GameMode::LoadMap(const std::string& filename) {
    if (mapRenderer.LoadMap(filename)) {
        currentMapFile = filename;
        isInitialized = true;

        // Set up camera bounds based on map
        sf::FloatRect mapBounds = mapRenderer.GetMapBounds();
        camera.setBounds(mapBounds.left, mapBounds.top,
            mapBounds.left + mapBounds.width,
            mapBounds.top + mapBounds.height);

        // If player exists, position them at spawn point
        if (player) {
            sf::Vector2f spawnPos = GetPlayerSpawnPosition();
            player->setPosition(spawnPos);
            camera.setTarget(player);
        }

        std::cout << "Game mode initialized with map: " << filename << std::endl;
        return true;
    }

    std::cerr << "Failed to initialize game mode with map: " << filename << std::endl;
    return false;
}

void GameMode::SetMapData(const MapData& data) {
    mapRenderer.SetMapData(data);
    isInitialized = true;

    // Set up camera bounds
    sf::FloatRect mapBounds = mapRenderer.GetMapBounds();
    camera.setBounds(mapBounds.left, mapBounds.top,
        mapBounds.left + mapBounds.width,
        mapBounds.top + mapBounds.height);

    // Position player at spawn point
    if (player) {
        sf::Vector2f spawnPos = GetPlayerSpawnPosition();
        player->setPosition(spawnPos);
        camera.setTarget(player);
    }
}

void GameMode::SetPlayer(Player* p) {
    player = p;
    if (player && isInitialized) {
        sf::Vector2f spawnPos = GetPlayerSpawnPosition();
        player->setPosition(spawnPos);
        camera.setTarget(player);
    }
}

void GameMode::Update(float deltaTime) {
    if (!isInitialized || isPaused) {
        return;
    }

    // Update player
    if (player) {
        player->update();

        // Check collision with map
        sf::Vector2f playerPos = player->getPosition();
        if (!IsValidPosition(playerPos)) {
            // Handle collision - could push player back or handle differently
            // For now, we'll just prevent movement into walls
        }
    }

    // Update camera
    camera.update();

    // Update debug info
    if (showDebugInfo) {
        UpdateDebugInfo();
    }
}

void GameMode::Render(sf::RenderWindow& window) {
    if (!isInitialized) {
        return;
    }

    // Set camera view
    sf::Vector2f windowSize(static_cast<float>(window.getSize().x),
        static_cast<float>(window.getSize().y));
    window.setView(camera.getView(windowSize));

    // Render map
    mapRenderer.Render(window, camera);

    // Render player
    if (player) {
        player->render(window);
    }

    // Reset view for UI
    window.setView(window.getDefaultView());

    // Render UI
    RenderUI(window);
}

void GameMode::HandleEvent(const sf::Event& event) {
    if (event.type == sf::Event::KeyPressed) {
        switch (event.key.code) {
        case sf::Keyboard::F1:
            ToggleDebugInfo();
            break;
        case sf::Keyboard::P:
        case sf::Keyboard::Pause:
            if (isPaused) {
                Resume();
            }
            else {
                Pause();
            }
            break;
        default:
            // Pass other events to player
            if (player) {
                // Player should handle its own input
            }
            break;
        }
    }
}

sf::Vector2f GameMode::GetPlayerSpawnPosition() const {
    if (!isInitialized) {
        return sf::Vector2f(0.0f, 0.0f);
    }

    const MapData& mapData = mapRenderer.GetMapData();

    // Look for a spawn tile or empty space
    for (int y = 0; y < mapData.height; ++y) {
        for (int x = 0; x < mapData.width; ++x) {
            TileType tile = mapData.tiles[y][x];
            if (tile == TileType::EMPTY || tile == TileType::TORCH) {
                // Check if there's ground below (for platformer-style games)
                if (y + 1 < mapData.height &&
                    mapData.tiles[y + 1][x] != TileType::EMPTY) {
                    return mapRenderer.GetWorldPosition(x, y);
                }
            }
        }
    }

    // If no suitable spawn found, use center of map
    sf::FloatRect bounds = mapRenderer.GetMapBounds();
    return sf::Vector2f(bounds.left + bounds.width * 0.5f,
        bounds.top + bounds.height * 0.5f);
}

bool GameMode::IsValidPosition(const sf::Vector2f& position) const {
    if (!isInitialized) {
        return false;
    }

    // Convert world position to grid coordinates
    sf::Vector2i gridPos = mapRenderer.GetGridPosition(position);
    TileType tile = mapRenderer.GetTileAt(gridPos.x, gridPos.y);

    // Check if position is walkable
    return tile == TileType::EMPTY || tile == TileType::TORCH;
}

TileType GameMode::GetTileAtPosition(const sf::Vector2f& position) const {
    if (!isInitialized) {
        return TileType::EMPTY;
    }

    sf::Vector2i gridPos = mapRenderer.GetGridPosition(position);
    return mapRenderer.GetTileAt(gridPos.x, gridPos.y);
}

void GameMode::UpdateDebugInfo() {
    std::ostringstream oss;
    oss << "=== GAME MODE DEBUG INFO ===\n";
    oss << "Map: " << currentMapFile << "\n";
    oss << "Map Size: " << mapRenderer.GetMapSize().x << "x" << mapRenderer.GetMapSize().y << "\n";
    oss << "Tile Size: " << mapRenderer.GetTileSize() << "\n";
    oss << "Paused: " << (isPaused ? "Yes" : "No") << "\n";
    oss << "Game Time: " << gameTimer.getElapsedTime().asSeconds() << "s\n";

    if (player) {
        sf::Vector2f playerPos = player->getPosition();
        sf::Vector2i gridPos = mapRenderer.GetGridPosition(playerPos);
        oss << "Player Pos: (" << playerPos.x << ", " << playerPos.y << ")\n";
        oss << "Player Grid: (" << gridPos.x << ", " << gridPos.y << ")\n";
        oss << "Tile At Player: " << static_cast<int>(GetTileAtPosition(playerPos)) << "\n";
    }
    else {
        oss << "Player: Not set\n";
    }

    oss << "\n[F1] Toggle Debug | [P] Pause/Resume";

    debugText.setString(oss.str());
}

void GameMode::RenderUI(sf::RenderWindow& window) {
    if (isPaused) {
        // Render pause overlay
        sf::RectangleShape overlay;
        overlay.setSize(sf::Vector2f(static_cast<float>(window.getSize().x),
            static_cast<float>(window.getSize().y)));
        overlay.setFillColor(sf::Color(0, 0, 0, 128));
        window.draw(overlay);

        sf::Text pauseText;
        pauseText.setFont(font);
        pauseText.setString("PAUSED\nPress P to resume");
        pauseText.setCharacterSize(32);
        pauseText.setFillColor(sf::Color::White);

        sf::FloatRect textBounds = pauseText.getLocalBounds();
        pauseText.setPosition(
            (window.getSize().x - textBounds.width) * 0.5f,
            (window.getSize().y - textBounds.height) * 0.5f
        );

        window.draw(pauseText);
    }

    if (showDebugInfo) {
        window.draw(debugText);
    }
}