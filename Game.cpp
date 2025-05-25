#include "stdafx.h"
#include "Game.h"
#include "Map.h"

Map map(32.0f);
Camera camera(300.0f); // Zoom más pequeño para mejor rendimiento

const float movementSpeed = 200.0f;

void init(const sf::Window& window)
{
    std::cout << "Cargando texturas..." << std::endl;

    // Cargar solo texturas esenciales
    try {
        for (auto& file : std::filesystem::directory_iterator("./assets/World/"))
        {
            if (file.path().extension() == ".png")
            {
                std::string name = file.path().filename().string();
                name = name.substr(0, name.find_last_of('.'));

                if (Resources::textures[name].loadFromFile(file.path().string())) {
                    std::cout << "Textura cargada: " << name << std::endl;
                }
                else {
                    std::cout << "Error cargando: " << name << std::endl;
                }
            }
        }
    }
    catch (...) {
        std::cout << "Error accediendo a directorio de texturas" << std::endl;
    }

    // Crear texturas básicas si no existen
    if (Resources::textures.find("Tile") == Resources::textures.end()) {
        sf::Image img;
        img.create(32, 32, sf::Color::White);
        Resources::textures["Tile"].loadFromImage(img);
        std::cout << "Textura Tile creada por defecto" << std::endl;
    }

    // Crear un mapa pequeño para testing
    map.grid.clear();
    map.grid.resize(30, std::vector<int>(20, 0));

    // Crear algunos bloques de prueba
    for (int x = 10; x < 20; ++x) {
        for (int y = 10; y < 15; ++y) {
            map.grid[x][y] = 1;
        }
    }

    camera.position = sf::Vector2f(400.0f, 300.0f);
    std::cout << "Inicialización completada" << std::endl;
}

void HandleInput(const sf::Event& event, const sf::RenderWindow& window)
{
    // Simplificado por ahora
}

void Update(float deltaTime)
{
    // Movimiento básico
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

void RenderUI(sf::RenderWindow& window)
{
    // UI simplificada por ahora
}