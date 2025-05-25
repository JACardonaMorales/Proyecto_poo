#include "stdafx.h"
#include "Game.h"
#include <iostream>

int main()
{
    std::cout << "Iniciando juego..." << std::endl;

    sf::RenderWindow window(sf::VideoMode(800, 600), "Juego - Debugging");
    window.setFramerateLimit(60);

    std::cout << "Ventana creada, inicializando..." << std::endl;

    try {
        init(window);
        std::cout << "Inicialización completada" << std::endl;
    }
    catch (const std::exception& e) {
        std::cout << "Error en inicialización: " << e.what() << std::endl;
        return -1;
    }

    sf::Clock clock;
    int frameCount = 0;

    while (window.isOpen())
    {
        float deltaTime = clock.restart().asSeconds();
        frameCount++;

        // Debug FPS cada segundo
        if (frameCount % 60 == 0) {
            std::cout << "FPS: " << 1.0f / deltaTime << std::endl;
        }

        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();

            // Solo usar editor si no causa problemas
            try {
                HandleInput(event, window);
            }
            catch (...) {
                std::cout << "Error en HandleInput" << std::endl;
            }
        }

        try {
            Update(deltaTime);
        }
        catch (...) {
            std::cout << "Error en Update" << std::endl;
        }

        // Vista de cámara más conservadora
        window.setView(camera.getView(sf::Vector2f(800, 600)));

        window.clear(sf::Color::Black);

        try {
            Renderer renderer(window);
            Render(renderer);
        }
        catch (...) {
            std::cout << "Error en Render" << std::endl;
        }

        try {
            RenderUI(window);
        }
        catch (...) {
            // Ignorar errores de UI por ahora
        }

        window.display();
    }

    std::cout << "Juego terminado" << std::endl;
    return 0;
}