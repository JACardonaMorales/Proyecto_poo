#include "stdafx.h"
#include "Game.h"
#include "Map.h"


Map map(32.0f);
Camera camera(3200.0f);

const float movementSpeed = 100.0f;

void init(const sf::Window& window)
{
	for (auto& file : std::filesystem::directory_iterator("./assets/World/"))
	{
		if (file.path().extension() == ".png")
		{
			std::string name = file.path().filename().string();
			name = name.substr(0, name.find_last_of('.'));
			Resources::textures[name].loadFromFile(file.path().string());
		}
	}

	sf::Image image;
	image.loadFromFile("./assets/World/world.png");
	map.InitFromImage(image);

	camera.position = sf::Vector2f(160.0f, 160.0f);
}

void Update(float deltaTime)
{
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
		camera.position.x += movementSpeed * deltaTime;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
		camera.position.x -= movementSpeed * deltaTime;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
		camera.position.y -= movementSpeed * deltaTime;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
		camera.position.y += movementSpeed * deltaTime;
}

void Render(Renderer& renderer)
{
	map.Draw(renderer);
}

