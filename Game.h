#pragma once
#include <SFML/Graphics.hpp>
#include "Player.h"
#include "Camera.h"
#include "Renderer.h"
#include "Resources.h"
#include "Map.h"
#include "Physics.h"
#include "LevelEditor.h"

// Forward declaration
class LevelEditor;

extern Camera camera;
extern Player player;
extern Map gameMap;
extern LevelEditor levelEditor;
extern bool showLevelEditor;

void init(const sf::Window& window);
void HandleInput(const sf::Event& event, const sf::RenderWindow& window);
void Update(float deltaTime);
void Render(Renderer& renderer);
void RenderUI(sf::RenderWindow& window);
void SyncMapWithEditor();