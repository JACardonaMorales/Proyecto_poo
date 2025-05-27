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
#include "Resources.h"
#include "SpriteLoader.h"
#include "Physics.h"


// Estructura para representar el mapa del juego
struct map {
    std::vector<std::vector<int>> grid;
    float cellSize;

    map() : cellSize(32.0f) {}
};

// Variables globales del juego
extern Player player;
extern Camera camera;
extern LevelEditor levelEditor;
extern Map currentMap;
extern bool showLevelEditor;
extern bool gameInitialized;

// Funciones principales del juego
void init(sf::RenderWindow& window);
void HandleInput(const sf::Event& event, const sf::RenderWindow& window);
void Update(float deltaTime);
void Render(Renderer& renderer);
void RenderUI(sf::RenderWindow& window);

// Funciones de inicialización
void LoadGameResources();
void CreateDefaultPlayerSprite();
void InitializePlayer();
void LoadDefaultMap();
void InitializeCamera();

// Funciones de manejo de mapas
bool LoadMapFromLevelEditor(const std::string& filename);
void CreateBasicMap();
void SyncMapWithEditor();

// Funciones de renderizado
void RenderMap(Renderer& renderer);
void RenderMapTile(Renderer& renderer, int tileType, const sf::Vector2f& position);
sf::Vector2f GetWorldPositionFromGrid(int gridX, int gridY);
