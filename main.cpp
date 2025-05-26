#include "stdafx.h"
#include "Game.h"

int main()
{
    // Crear ventana
    sf::RenderWindow window(sf::VideoMode(1200, 800), "Dead Paradise");
    window.setFramerateLimit(60);

    // Inicializar el juego
    init(window);

    // Variables para el tiempo
    sf::Clock clock;

    // Bucle principal
    while (window.isOpen()) {
        float deltaTime = clock.restart().asSeconds();

        // Manejar eventos
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }

            // Pasar eventos al juego
            HandleInput(event, window);
        }

        // Actualizar
        Update(deltaTime);

        // Configurar la vista de la cámara
        window.setView(camera.getView(sf::Vector2f(window.getSize())));

        // Limpiar pantalla
        window.clear(sf::Color(50, 50, 100)); // Fondo azul oscuro

        // Crear renderer y dibujar
        Renderer renderer(window);
        Render(renderer);

        // Dibujar UI (se dibuja después de restaurar la vista por defecto)
        RenderUI(window);

        // Mostrar
        window.display();
    }

    return 0;
}