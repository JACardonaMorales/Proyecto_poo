#include "stdafx.h"
#include "Game.h"
#include "LevelEditor.h"

// Variables globales
Camera camera(10.0f);
Player player;
Map gameMap(32.0f);
LevelEditor* levelEditor = nullptr;

void init(const sf::Window& window)
{
	Physics::Init(); // Inicializar el mundo de Box2D

    // Inicializar el mapa con un tamaño por defecto
    gameMap.createBoard(100, 50);

    // Crear el editor de niveles
    levelEditor = new LevelEditor(gameMap, 32.0f);

    // Cargar recursos si es necesario
    // Resources::textures["player"].loadFromFile("assets/player.png");
    // etc...

    std::cout << "Game initialized" << std::endl;
    std::cout << "Press E to toggle Level Editor" << std::endl;

    player.init;
}

void HandleInput(const sf::Event& event, const sf::RenderWindow& window)
{
    // El editor maneja su propia entrada
    if (levelEditor) {
        levelEditor->HandleInput(event, window);
    }

    // Solo procesar entrada del juego si no estamos en modo editor
    if (!levelEditor || !levelEditor->IsEditorMode()) {
        // Aquí irían los controles del jugador
        if (event.type == sf::Event::KeyPressed) {
            switch (event.key.code) {
            case sf::Keyboard::W:
                // Mover jugador hacia arriba
                break;
            case sf::Keyboard::S:
                // Mover jugador hacia abajo
                break;
            case sf::Keyboard::A:
                // Mover jugador hacia la izquierda
                break;
            case sf::Keyboard::D:
                // Mover jugador hacia la derecha
                break;
            }
        }
    }

    // Controles de cámara (funcionan siempre)
    if (event.type == sf::Event::KeyPressed) {
        switch (event.key.code) {
        case sf::Keyboard::Up:
            camera.position.y -= 32.0f;
            break;
        case sf::Keyboard::Down:
            camera.position.y += 32.0f;
            break;
        case sf::Keyboard::Left:
            camera.position.x -= 32.0f;
            break;
        case sf::Keyboard::Right:
            camera.position.x += 32.0f;
            break;
        case sf::Keyboard::Z:
            camera.zoomLevel *= 0.9f; // Zoom in
            break;
        case sf::Keyboard::X:
            camera.zoomLevel *= 1.1f; // Zoom out
            break;
        }
    }
}

void Update(float deltaTime)
{
    // Actualizar el editor
    if (levelEditor) {
        levelEditor->Update(deltaTime);
    }

    // Solo actualizar lógica del juego si no estamos en modo editor
    if (!levelEditor || !levelEditor->IsEditorMode()) {
        // Aquí iría la lógica de actualización del juego
        // player.Update(deltaTime);
        // Física, colisiones, etc.
    }
}

void Render(Renderer& renderer)
{
    // Dibujar el mapa
    gameMap.Draw(renderer);

    // Solo dibujar elementos del juego si no estamos en modo editor
    if (!levelEditor || !levelEditor->IsEditorMode()) {
        // Dibujar jugador y otros elementos del juego
        // player.Draw(renderer);
    }

    // Dibujar overlay del editor (grid, etc.)
    if (levelEditor) {
        levelEditor->Draw(renderer);
    }
}

void RenderUI(sf::RenderWindow& window)
{
    // Dibujar UI del editor
    if (levelEditor) {
        levelEditor->DrawUI(window);
    }

    // Dibujar otra UI del juego si es necesario
    if (!levelEditor || !levelEditor->IsEditorMode()) {
        // UI del juego normal (HUD, etc.)
    }
}