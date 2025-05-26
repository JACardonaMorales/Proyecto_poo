// Ejemplo de integración en Game.cpp

#include "stdafx.h"
#include "Game.h"
#include "Physics.h"

// Variables globales
Player player;
Map gameMap;
LevelEditor levelEditor(gameMap);
Camera camera;
void handlePlayerDamage(TileType damageSource);

void init(const sf::Window& window) {
    gameMap.createBoard(50, 30); // Crear un mapa de 50x30

    // Cargar nivel por defecto o crear uno nuevo
    // gameMap.InitFromImage(someImage);
}

void HandleInput(const sf::Event& event, const sf::RenderWindow& window) {
    // Manejar input del editor de niveles
    levelEditor.HandleInput(event, window);

    // Si estamos en modo editor, no procesar input del jugador
    if (levelEditor.IsEditorMode()) {
        return;
    }

    // Input del jugador
    if (event.type == sf::Event::KeyPressed) {
        switch (event.key.code) {
        case sf::Keyboard::F1:
            levelEditor.SetEditorMode(true);
            break;
        case sf::Keyboard::R:
            // Reiniciar jugador
            player.setPosition(100, 100);
            break;
        }
    }
}

void Update(float deltaTime) {
    if (levelEditor.IsEditorMode()) {
        levelEditor.Update(deltaTime);
        return;
    }

    // Actualizar jugador
    player.update();

    // Verificar colisiones
    CollisionInfo collision = Physics::CheckCollision(player, gameMap);

    if (collision.hasCollision) {
        // Aplicar corrección de posición
        sf::Vector2f currentPos = player.getPosition();
        player.setPosition(currentPos.x + collision.correctionVector.x,
            currentPos.y + collision.correctionVector.y);

        // Actualizar estados del jugador basado en la colisión
        player.setIsOnGround(collision.isGrounded);
        player.setIsOnPlatform(collision.isOnPlatform);
        player.setIsOnLadder(collision.isOnLadder);

        // Si hay corrección vertical, resetear velocidad Y
        if (collision.correctionVector.y != 0) {
            player.resetVelocityY();
        }
    }
    else {
        // No hay colisión, el jugador no está en el suelo
        player.setIsOnGround(false);
        player.setIsOnPlatform(false);
        player.setIsOnLadder(false);
    }

    // Verificar daño
    if (collision.takeDamage) {
        // Implementar lógica de daño
        handlePlayerDamage(collision.tileType);
    }

    // Actualizar cámara para seguir al jugador
    camera.position = player.getPosition();
}

void Render(Renderer& renderer) {
    // Dibujar mapa
    gameMap.Draw(renderer);

    // Dibujar jugador
    // renderer.Draw(...) // Implementar según tu sistema de renderizado

    // Dibujar editor si está activo
    if (levelEditor.IsEditorMode()) {
        levelEditor.Draw(renderer);
    }
}

void RenderUI(sf::RenderWindow& window) {
    if (levelEditor.IsEditorMode()) {
        levelEditor.DrawUI(window);
    }

    // Dibujar UI del juego (vida, puntuación, etc.)
    // ...
}

void handlePlayerDamage(TileType damageSource) {
    switch (damageSource) {
    case TileType::SPIKE:
        // Lógica específica para daño de pinchos
        std::cout << "Player hit spikes!" << std::endl;
        // Reducir vida, reiniciar posición, etc.
        break;
    default:
        break;
    }
}

// Funciones de utilidad adicionales
void SaveCurrentLevel() {
    levelEditor.SaveLevel("current_level.dat");
}

void LoadLevel(const std::string& filename) {
    levelEditor.LoadLevel(filename);
}

void CreateTestLevel() {
    // Crear un nivel de prueba con diferentes tipos de tiles
    gameMap.createBoard(20, 15);
    levelEditor.ClearLevel();

    // Agregar algunos tiles de ejemplo
    gameMap.grid[10][5] = static_cast<int>(TileType::SOLID_BLOCK);
    gameMap.grid[10][6] = static_cast<int>(TileType::PLATFORM);
    gameMap.grid[9][7] = static_cast<int>(TileType::SPIKE);
    gameMap.grid[8][8] = static_cast<int>(TileType::TORCH);
    gameMap.grid[10][10] = static_cast<int>(TileType::DOOR);

    // Crear una pequeña estructura con el tileset
    for (int x = 12; x < 15; x++) {
        for (int y = 8; y < 11; y++) {
            TileType tileType = TileType::BLOCK_CENTER;

            // Esquinas y bordes
            if (x == 12 && y == 8) tileType = TileType::BLOCK_TOP_LEFT;
            else if (x == 14 && y == 8) tileType = TileType::BLOCK_TOP_RIGHT;
            else if (x == 12 && y == 10) tileType = TileType::BLOCK_BOTTOM_LEFT;
            else if (x == 14 && y == 10) tileType = TileType::BLOCK_BOTTOM_RIGHT;
            else if (y == 8) tileType = TileType::BLOCK_TOP;
            else if (y == 10) tileType = TileType::BLOCK_BOTTOM;
            else if (x == 12) tileType = TileType::BLOCK_LEFT;
            else if (x == 14) tileType = TileType::BLOCK_RIGHT;

            gameMap.grid[y][x] = static_cast<int>(tileType);
        }
    }
}