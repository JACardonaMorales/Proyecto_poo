#include "stdafx.h"
#include "Game.h"
#include "SpriteLoader.h"

// Global instances
Camera camera(5.0f);
Player player;
Map gameMap(32.0f);
LevelEditor levelEditor;
bool showLevelEditor = false;

void init(const sf::Window& window)
{
    // Initialize SpriteLoader and load resources
    SpriteLoader& spriteLoader = SpriteLoader::getInstance();

    // Load platform sprite (try to load from file, fallback to default)
    spriteLoader.LoadPlatformSprite("assets/sprites/platform.png");

    // Load other sprites
    spriteLoader.LoadSprite("wall", "assets/sprites/wall.png");
    spriteLoader.LoadSprite("spikes", "assets/sprites/spike.png");
    spriteLoader.LoadSprite("door", "assets/sprites/door.png");

    // Load tileset
    spriteLoader.LoadTileset("assets/sprites/tileset.png", 16, 16);

    // Register torch animation
    SpriteLoader::AnimationData torchAnim;
    torchAnim.textureName = "torch";
    torchAnim.frameCount = 8;
    torchAnim.frameWidth = 16;
    torchAnim.frameHeight = 32;
    torchAnim.frameTime = 0.125f; // 8 FPS
    torchAnim.loop = true;
    spriteLoader.RegisterAnimation("torch_burn", torchAnim);

    // Load torch spritesheet
    spriteLoader.LoadSprite("torch", "assets/sprites/torch.png");

    // Initialize level editor
    levelEditor.Initialize();

    // Create a default level or load existing one
    gameMap.createBoard(50, 30);

    // Try to load a level file if it exists
    if (std::filesystem::exists("level.txt")) {
        levelEditor.LoadLevel("level.txt");
        // Copy level editor data to game map
        const auto& editorGrid = levelEditor.GetGrid();
        gameMap.createBoard(levelEditor.GetGridWidth(), levelEditor.GetGridHeight());

        for (int y = 0; y < levelEditor.GetGridHeight(); ++y) {
            for (int x = 0; x < levelEditor.GetGridWidth(); ++x) {
                gameMap.grid[y][x] = static_cast<int>(editorGrid[y][x]);
            }
        }
    }

    // Set initial player position
    player.setPosition(0.0f, -200.0f);

    std::cout << "Game initialized successfully!" << std::endl;
}

void HandleInput(const sf::Event& event, const sf::RenderWindow& window)
{
    // Toggle level editor
    if (event.type == sf::Event::KeyPressed) {
        if (event.key.code == sf::Keyboard::Tab) {
            showLevelEditor = !showLevelEditor;
            std::cout << "Level Editor " << (showLevelEditor ? "ON" : "OFF") << std::endl;
        }

        // Quick save/load when not in editor
        if (!showLevelEditor) {
            if (event.key.code == sf::Keyboard::F5) {
                levelEditor.SaveLevel("quicksave.txt");
                std::cout << "Quick saved to quicksave.txt" << std::endl;
            }
            if (event.key.code == sf::Keyboard::F9) {
                levelEditor.LoadLevel("quicksave.txt");
                SyncMapWithEditor();
                std::cout << "Quick loaded from quicksave.txt" << std::endl;
            }
        }
    }

    // Handle level editor input
    if (showLevelEditor) {
        levelEditor.HandleInput(event, window);
    }
    else {
        // Handle normal game input here
        // Player movement will be handled in Update()
    }
}

void Update(float deltaTime)
{
    if (showLevelEditor) {
        levelEditor.Update(deltaTime);

        // Sync map with level editor changes
        SyncMapWithEditor();
    }
    else {
        // Update player
        player.update();

        // Update camera to follow player
        camera.position = player.getPosition();

        // Simple boundary check
        sf::Vector2f playerPos = player.getPosition();
        float mapWidth = gameMap.grid.empty() ? 0 : gameMap.grid[0].size() * gameMap.cellSize;
        float mapHeight = gameMap.grid.size() * gameMap.cellSize;

        // Keep camera within reasonable bounds
        camera.position.x = std::max(-mapWidth / 2.0f, std::min(mapWidth / 2.0f, camera.position.x));
        camera.position.y = std::max(-mapHeight / 2.0f, std::min(mapHeight / 2.0f, camera.position.y));
    }
}

void Render(Renderer& renderer)
{
    // Always render the map
    gameMap.Draw(renderer);

    if (!showLevelEditor) {
        // Render player when not in editor mode
        // Note: Player render needs to be adapted to work with Renderer class
        // For now, we'll skip player rendering in this context
        // player.render(target); // This would need the actual render target
    }
}

void RenderUI(sf::RenderWindow& window)
{
    if (showLevelEditor) {
        levelEditor.Render(window);
    }
    else {
        // Render game UI
        sf::Font font;
        sf::Text gameInfo;

        // Try to load font, use default if failed
        if (font.loadFromFile("assets/font.ttf")) {
            gameInfo.setFont(font);
        }

        gameInfo.setCharacterSize(16);
        gameInfo.setFillColor(sf::Color::White);
        gameInfo.setPosition(10, 10);

        sf::Vector2f playerPos = player.getPosition();
        std::string info = "Player Position: (" +
            std::to_string((int)playerPos.x) + ", " +
            std::to_string((int)playerPos.y) + ")\n";
        info += "Controls: WASD - Move, Space - Jump, Tab - Toggle Editor\n";
        info += "F5 - Quick Save, F9 - Quick Load";

        gameInfo.setString(info);
        window.draw(gameInfo);
    }
}

void SyncMapWithEditor()
{
    // Copy level editor data to game map
    const auto& editorGrid = levelEditor.GetGrid();
    int editorWidth = levelEditor.GetGridWidth();
    int editorHeight = levelEditor.GetGridHeight();

    // Resize game map if needed
    if (gameMap.grid.empty() ||
        gameMap.grid[0].size() != editorWidth ||
        gameMap.grid.size() != editorHeight) {
        gameMap.createBoard(editorWidth, editorHeight);
    }

    // Copy tile data
    for (int y = 0; y < editorHeight; ++y) {
        for (int x = 0; x < editorWidth; ++x) {
            gameMap.grid[y][x] = static_cast<int>(editorGrid[y][x]);
        }
    }
}