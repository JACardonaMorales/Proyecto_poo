#pragma once
#include"Player.h"
#include"Camera.h"
#include "Renderer.h"
#include "Resources.h"
#include "Map.h"
#include "LevelEditor.h"
#include "Physics.h"

extern Camera camera;

void init(const sf::Window& window);
void HandleInput(const sf::Event& event, const sf::RenderWindow& window);
void Update(float deltaTime);
void Render(Renderer& renderer);
void RenderUI(sf::RenderWindow& window);