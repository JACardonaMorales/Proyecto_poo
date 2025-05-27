// Game.h - Declaraciones del sistema principal del juego
#pragma once

#include "Player.h"
#include "Camera.h"
#include "Map.h"
#include "MapRenderer.h"
#include "Resources.h"

struct MapData;
class MapRenderer;

class GameMode{
private:
    MapRenderer mapRenderer;
	Camera camera;
    Player* player;

    bool isInitialized;
    std::string currentMapFile;

    // Game state
    bool isPaused;
    sf::Clock gameTimer;

    // UI elements for game mode
    sf::Font font;
    sf::Text debugText;
    bool showDebugInfo;

    void InitializeUI();
    void UpdateDebugInfo();
    void RenderUI(sf::RenderWindow& window);

public:
    GameMode();
    ~GameMode();

    // Map management
    bool LoadMap(const std::string& filename);
    void SetMapData(const MapData& data);

    // Player management
    void SetPlayer(Player* p);
    Player* GetPlayer() const { return player; }

    // Game loop
    void Update(float deltaTime);
    void Render(sf::RenderWindow& window);
    void HandleEvent(const sf::Event& event);

    // Game state
    void Pause() { isPaused = true; }
    void Resume() { isPaused = false; }
    bool IsPaused() const { return isPaused; }

    // Utility
    sf::Vector2f GetPlayerSpawnPosition() const;
    bool IsValidPosition(const sf::Vector2f& position) const;
    TileType GetTileAtPosition(const sf::Vector2f& position) const;

    
    // Map info
    const MapRenderer& GetMapRenderer() const { return mapRenderer; }
    sf::FloatRect GetMapBounds() const { return mapRenderer.GetMapBounds(); }

    // Debug
    void ToggleDebugInfo() { showDebugInfo = !showDebugInfo; }
    bool IsDebugInfoVisible() const { return showDebugInfo; }
};