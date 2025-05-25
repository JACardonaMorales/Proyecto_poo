#pragma once
#include"Player.h"
#include"Camera.h"
#include "Renderer.h"
#include "Resources.h"
#include "Map.h"

extern Camera camera;

void init(const sf::Window& window);
void Update(float deltaTime);
void Render(Renderer& renderer);


