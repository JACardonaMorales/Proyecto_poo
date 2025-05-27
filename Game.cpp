// Game.cpp - Implementación completa del sistema principal
#include "stdafx.h"
#include "Game.h"
#include "Resources.h"
#include "Physics.h"
#include "SpriteLoader.h"

// Variables globales del juego
Player player;
Camera camera;
LevelEditor levelEditor;
Map currentMap;
bool showLevelEditor = false;
bool gameInitialized = false;

void init(sf::RenderWindow& window)
{
    std::cout << "Initializing game..." << std::endl;

    // Cargar recursos básicos
    LoadGameResources();

    // Inicializar el jugador
    InitializePlayer();

    // Cargar el mapa por defecto
    LoadDefaultMap();

    // Inicializar la cámara
    InitializeCamera();

    // Inicializar el editor de niveles
    levelEditor.SetActive(false);

    gameInitialized = true;
    std::cout << "Game initialized successfully!" << std::endl;
}

void LoadGameResources()
{
    std::cout << "Loading game resources..." << std::endl;

    // Cargar sprite del jugador
    if (!Resources::LoadTexture("RaySprite", "assets/sprites/ray.png")) {
        // Crear sprite por defecto del jugador si no se encuentra el archivo
        CreateDefaultPlayerSprite();
    }

    // Usar SpriteLoader para cargar recursos adicionales
    SpriteLoader& loader = SpriteLoader::getInstance();

    // Cargar sprites del nivel editor
    loader.LoadSprite("wall", "assets/sprites/wall.png");
    loader.LoadSprite("spikes", "assets/sprites/spike.png");
    loader.LoadSprite("torch", "assets/sprites/torch.png");
    loader.LoadSprite("door", "assets/sprites/door.png");

    // Cargar tileset
    loader.LoadTileset("assets/sprites/tileset.png", 32, 32);

    // Cargar sprite de plataforma (o crear uno por defecto)
    loader.LoadPlatformSprite("assets/sprites/platform.png");

    std::cout << "Resources loaded!" << std::endl;
}

void CreateDefaultPlayerSprite()
{
    std::cout << "Creating default player sprite..." << std::endl;

    sf::Image playerImage;
    playerImage.create(32, 32, sf::Color::Transparent);

    // Crear un sprite simple del jugador (cuadrado azul con detalles)
    for (int x = 8; x < 24; ++x) {
        for (int y = 8; y < 24; ++y) {
            playerImage.setPixel(x, y, sf::Color::Blue);
        }
    }

    // Agregar algunos detalles
    // Ojos
    playerImage.setPixel(12, 12, sf::Color::White);
    playerImage.setPixel(20, 12, sf::Color::White);
    playerImage.setPixel(12, 13, sf::Color::Black);
    playerImage.setPixel(20, 13, sf::Color::Black);

    // Sonrisa
    for (int x = 14; x < 18; ++x) {
        playerImage.setPixel(x, 18, sf::Color::White);
    }

    sf::Texture playerTexture;
    playerTexture.loadFromImage(playerImage);
    Resources::AddTexture("RaySprite", playerTexture);

    std::cout << "Default player sprite created!" << std::endl;
}

void InitializePlayer()
{
    std::cout << "Initializing player..." << std::endl;

    // El constructor del Player ya inicializa todo
    // Solo necesitamos posicionarlo
    player.setPosition(0.0f, 0.0f); // Posición inicial en el centro del mundo

    std::cout << "Player initialized at position (0, 0)" << std::endl;
}

void LoadDefaultMap()
{
    std::cout << "Loading default map..." << std::endl;

    // Cargar el mapa desde el archivo level.txt
    if (LoadMapFromLevelEditor("level.txt")) {
        std::cout << "Map loaded from level.txt successfully!" << std::endl;
    }
    else {
        // Crear un mapa básico si no se puede cargar el archivo
        CreateBasicMap();
        std::cout << "Created basic default map" << std::endl;
    }
}

bool LoadMapFromLevelEditor(const std::string& filename)
{
    std::ifstream file(filename);
    if (!file.is_open()) {
        return false;
    }

    int width, height;
    file >> width >> height;

    currentMap.grid.clear();
    currentMap.grid.resize(height, std::vector<int>(width));
    currentMap.cellSize = 32.0f;

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            file >> currentMap.grid[y][x];
        }
    }

    file.close();
    return true;
}

void CreateBasicMap()
{
    // Crear un mapa básico de 50x30 con algunas plataformas
    currentMap.grid.clear();
    currentMap.grid.resize(30, std::vector<int>(50, 0)); // 30 filas, 50 columnas
    currentMap.cellSize = 32.0f;

    // Agregar suelo en la parte inferior
    for (int x = 0; x < 50; ++x) {
        currentMap.grid[28][x] = 14; // BLOCK_CENTER
        currentMap.grid[29][x] = 14; // BLOCK_CENTER
    }

    // Agregar algunas plataformas
    for (int x = 10; x < 20; ++x) {
        currentMap.grid[25][x] = 2; // PLATFORM
    }

    for (int x = 30; x < 40; ++x) {
        currentMap.grid[22][x] = 2; // PLATFORM
    }

    // Agregar algunos obstáculos
    currentMap.grid[27][15] = 3; // SPIKES
    currentMap.grid[27][35] = 3; // SPIKES

    // Agregar una antorcha
    currentMap.grid[24][25] = 4; // TORCH
}

void InitializeCamera()
{
    camera.setTarget(&player);
    camera.setBounds(-800.0f, -480.0f, 800.0f, 480.0f); // Límites del mapa
}

void HandleInput(const sf::Event& event, const sf::RenderWindow& window)
{
    // Toggle del editor de niveles
    if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Tab) {
        showLevelEditor = !showLevelEditor;
        levelEditor.SetActive(showLevelEditor);

        if (showLevelEditor) {
            std::cout << "Level Editor enabled" << std::endl;
        }
        else {
            std::cout << "Level Editor disabled" << std::endl;
        }
    }

    // Quick save/load
    if (event.type == sf::Event::KeyPressed) {
        if (event.key.code == sf::Keyboard::F5) {
            levelEditor.SaveLevel("quicksave.txt");
            std::cout << "Quick saved!" << std::endl;
        }
        if (event.key.code == sf::Keyboard::F9) {
            levelEditor.LoadLevel("quicksave.txt");
            LoadMapFromLevelEditor("quicksave.txt");
            std::cout << "Quick loaded!" << std::endl;
        }
    }

    // Pasar input al editor si está activo
    if (showLevelEditor) {
        levelEditor.HandleInput(event, window);
    }
}

void Update(float deltaTime)
{
    if (!gameInitialized) return;

    if (showLevelEditor) {
        // Actualizar editor
        levelEditor.Update(deltaTime);

        // Sincronizar el mapa del juego con el editor si hay cambios
        SyncMapWithEditor();
    }
    else {
        // Actualizar juego normal
        player.update();

        // Aplicar físicas del jugador con el mapa
        Physics::HandlePlayerCollision(player, currentMap);

        // Actualizar cámara
        camera.update();
    }
}

void SyncMapWithEditor()
{
    // Sincronizar el mapa del juego con el grid del editor
    if (levelEditor.GetGridWidth() != currentMap.grid[0].size() ||
        levelEditor.GetGridHeight() != currentMap.grid.size()) {
        // Redimensionar si es necesario
        currentMap.grid.resize(levelEditor.GetGridHeight(),
            std::vector<int>(levelEditor.GetGridWidth()));
    }

    // Copiar datos del editor al mapa del juego
    for (int y = 0; y < levelEditor.GetGridHeight(); ++y) {
        for (int x = 0; x < levelEditor.GetGridWidth(); ++x) {
            currentMap.grid[y][x] = (int)levelEditor.GetTileAt(x, y);
        }
    }
}

void Render(Renderer& renderer)
{
    if (!gameInitialized) return;

    if (showLevelEditor) {
        // Renderizar editor
        levelEditor.Render(renderer.GetTarget());
    }
    else {
        // Renderizar mapa del juego
        RenderMap(renderer);

        // Renderizar jugador
        player.render(renderer.GetTarget());
    }
}

void RenderMap(Renderer& renderer)
{
    if (currentMap.grid.empty()) return;

    // Obtener los límites de la cámara para renderizar solo los tiles visibles
    sf::View currentView = renderer.GetTarget().getView();
    sf::Vector2f center = currentView.getCenter();
    sf::Vector2f size = currentView.getSize();

    float left = center.x - size.x / 2.0f;
    float right = center.x + size.x / 2.0f;
    float top = center.y - size.y / 2.0f;
    float bottom = center.y + size.y / 2.0f;

    // Convertir a coordenadas de grid
    int startX = std::max(0, (int)((left + 800.0f) / currentMap.cellSize));
    int endX = std::min((int)currentMap.grid[0].size(), (int)((right + 800.0f) / currentMap.cellSize) + 1);
    int startY = std::max(0, (int)((top + 480.0f) / currentMap.cellSize));
    int endY = std::min((int)currentMap.grid.size(), (int)((bottom + 480.0f) / currentMap.cellSize) + 1);

    // Renderizar tiles visibles
    for (int y = startY; y < endY; ++y) {
        for (int x = startX; x < endX; ++x) {
            if (currentMap.grid[y][x] != 0) { // No renderizar tiles vacíos
                sf::Vector2f worldPos = GetWorldPositionFromGrid(x, y);
                RenderMapTile(renderer, currentMap.grid[y][x], worldPos);
            }
        }
    }
}

void RenderMapTile(Renderer& renderer, int tileType, const sf::Vector2f& position)
{
    sf::Texture* texture = nullptr;
    TileType type = static_cast<TileType>(tileType);

    switch (type) {
    case TileType::WALL:
        texture = Resources::GetTexture("wall");
        break;
    case TileType::PLATFORM:
        texture = Resources::GetTexture("platform");
        break;
    case TileType::SPIKES:
        texture = Resources::GetTexture("spikes");
        break;
    case TileType::TORCH:
        texture = Resources::GetTexture("torch");
        break;
    case TileType::DOOR:
        texture = Resources::GetTexture("door");
        break;
    default:
        // Para tiles del tileset
        texture = Resources::GetTexture("tileset");
        break;
    }

    if (texture) {
        renderer.Draw(*texture, position, sf::Vector2f(currentMap.cellSize, currentMap.cellSize));
    }
}

sf::Vector2f GetWorldPositionFromGrid(int gridX, int gridY)
{
    float x = gridX * currentMap.cellSize - (currentMap.grid[0].size() * currentMap.cellSize) / 2.0f + currentMap.cellSize / 2.0f;
    float y = gridY * currentMap.cellSize - (currentMap.grid.size() * currentMap.cellSize) / 2.0f + currentMap.cellSize / 2.0f;
    return sf::Vector2f(x, y);
}

void RenderUI(sf::RenderWindow& window)
{
    if (!gameInitialized) return;

    // Mostrar información de estado
    static sf::Font font;
    static sf::Text statusText;
    static bool fontLoaded = false;

    if (!fontLoaded) {
        if (!font.loadFromFile("assets/arial.ttf")) {
            // Usar fuente por defecto del sistema si no se encuentra arial.ttf
        }
        statusText.setFont(font);
        statusText.setCharacterSize(16);
        statusText.setFillColor(sf::Color::White);
        fontLoaded = true;
    }

    std::string status = "Dead Paradise\n";
    status += "Player Position: (" + std::to_string((int)player.getPosition().x) + ", " + std::to_string((int)player.getPosition().y) + ")\n";
    status += "Velocity: (" + std::to_string(player.getVelocity().x) + ", " + std::to_string(player.getVelocity().y) + ")\n";

    if (showLevelEditor) {
        status += "LEVEL EDITOR MODE\n";
        status += "Press TAB to return to game\n";
    }
    else {
        status += "GAME MODE\n";
        status += "Press TAB for Level Editor\n";
        status += "WASD/Arrows: Move, Space: Jump\n";
        status += "F5: Quick Save, F9: Quick Load\n";
    }

    statusText.setString(status);
    statusText.setPosition(10, 10);
    window.draw(statusText);
}