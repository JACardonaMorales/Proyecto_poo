#include "stdafx.h"
#include "Game.h"
#include "LevelEditor.h"
#include "Player.h"
#include "Resources.h"
#include <SFML/Graphics.hpp>
#include <iostream>

enum class AppState {
    MainMenu,
    LevelEditor,
    GameMode,
    Exit
};

class Application {
private:
    sf::RenderWindow window;
    AppState currentState;
    AppState nextState;

    // Game components
    std::unique_ptr<LevelEditor> levelEditor;
    std::unique_ptr<GameMode> gameMode;
    std::unique_ptr<Player> player;

    // UI
    sf::Font font;
    sf::Text titleText;
    sf::Text menuText;
    sf::Text instructionsText;

    // Menu selection
    int selectedOption;
    const int numMenuOptions = 3;

    // Timing
    sf::Clock clock;
    float deltaTime;

    bool LoadResources();
    void InitializeUI();
    void UpdateMainMenu();
    void RenderMainMenu();
    void HandleMainMenuEvent(const sf::Event& event);
    void SwitchState(AppState newState);

public:
    Application();
    ~Application();

    bool Initialize();
    void Run();
    void HandleEvent(const sf::Event& event);
    void Update();
    void Render();
};

Application::Application()
    : currentState(AppState::MainMenu), nextState(AppState::MainMenu), selectedOption(0) {
}

Application::~Application() {
    Resources::Clear();
}

bool Application::Initialize() {
    // Create window
    window.create(sf::VideoMode(1200, 800), "Level Editor & Game", sf::Style::Default);
    window.setFramerateLimit(60);

    // Load resources
    if (!LoadResources()) {
        std::cerr << "Failed to load resources!" << std::endl;
        return false;
    }

    // Initialize UI
    InitializeUI();

    // Create game components
    levelEditor = std::make_unique<LevelEditor>();
    gameMode = std::make_unique<GameMode>();
    player = std::make_unique<Player>();

    // Set up player in game mode
    gameMode->SetPlayer(player.get());

    std::cout << "Application initialized successfully!" << std::endl;
    return true;
}

bool Application::LoadResources() {
    // Load essential textures for tiles
    if (!Resources::LoadTexture("tileset", "assets/textures/tileset.png")) {
        std::cerr << "Warning: Could not load tileset.png" << std::endl;

        // Create a simple fallback texture
        sf::Image fallbackImage;
        fallbackImage.create(256, 256, sf::Color::White);

        // Create some basic colored squares for different tiles
        for (int y = 0; y < 8; ++y) {
            for (int x = 0; x < 8; ++x) {
                sf::Color color;
                if (x == 0 && y == 0) color = sf::Color::Red;      // Wall
                else if (x == 1 && y == 0) color = sf::Color::Green;   // Floor
                else if (x == 2 && y == 0) color = sf::Color::Blue;    // Water
                else if (x == 3 && y == 0) color = sf::Color::Yellow;  // Torch
                else color = sf::Color(100, 100, 100);

                for (int py = 0; py < 32; ++py) {
                    for (int px = 0; px < 32; ++px) {
                        fallbackImage.setPixel(x * 32 + px, y * 32 + py, color);
                    }
                }
            }
        }

        sf::Texture fallbackTexture;
        fallbackTexture.loadFromImage(fallbackImage);
        Resources::AddTexture("tileset", fallbackTexture);
    }

    // Load player texture
    if (!Resources::LoadTexture("player", "assets/textures/player.png")) {
        std::cerr << "Warning: Could not load player.png" << std::endl;

        // Create simple player texture
        sf::Image playerImage;
        playerImage.create(32, 32, sf::Color::Cyan);
        sf::Texture playerTexture;
        playerTexture.loadFromImage(playerImage);
        Resources::AddTexture("player", playerTexture);
    }

    return true;
}

void Application::InitializeUI() {
    // Load font
    if (!font.loadFromFile("assets/fonts/arial.ttf")) {
        std::cout << "Warning: Could not load font, using default" << std::endl;
    }

    // Title text
    titleText.setFont(font);
    titleText.setString("LEVEL EDITOR & GAME");
    titleText.setCharacterSize(48);
    titleText.setFillColor(sf::Color::White);
    titleText.setStyle(sf::Text::Bold);

    // Menu text
    menuText.setFont(font);
    menuText.setCharacterSize(24);
    menuText.setFillColor(sf::Color::White);

    // Instructions text
    instructionsText.setFont(font);
    instructionsText.setCharacterSize(16);
    instructionsText.setFillColor(sf::Color(200, 200, 200));
    instructionsText.setString(
        "Controls:\n"
        "Arrow Keys - Navigate menu\n"
        "Enter - Select option\n"
        "Escape - Return to menu\n\n"
        "Level Editor:\n"
        "Mouse - Place/Remove tiles\n"
        "1-9 - Select tile type\n"
        "S - Save map\n"
        "L - Load map\n"
        "C - Clear map\n\n"
        "Game Mode:\n"
        "WASD/Arrow Keys - Move player\n"
        "F1 - Toggle debug info\n"
        "P - Pause/Resume"
    );
}

void Application::Run() {
    while (window.isOpen() && currentState != AppState::Exit) {
        // Handle state transitions
        if (nextState != currentState) {
            SwitchState(nextState);
        }

        // Calculate delta time
        deltaTime = clock.restart().asSeconds();

        // Handle events
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
                break;
            }

            HandleEvent(event);
        }

        // Update
        Update();

        // Render
        window.clear(sf::Color::Black);
        Render();
        window.display();
    }
}

void Application::HandleEvent(const sf::Event& event) {
    switch (currentState) {
    case AppState::MainMenu:
        HandleMainMenuEvent(event);
        break;

    case AppState::LevelEditor:
        if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) {
            nextState = AppState::MainMenu;
        }
        else {
            levelEditor->HandleInput(event, window);
        }
        break;

    case AppState::GameMode:
        if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) {
            nextState = AppState::MainMenu;
        }
        else {
            gameMode->HandleEvent(event);
        }
        break;

    default:
        break;
    }
}

void Application::HandleMainMenuEvent(const sf::Event& event) {
    if (event.type == sf::Event::KeyPressed) {
        switch (event.key.code) {
        case sf::Keyboard::Up:
            selectedOption = (selectedOption - 1 + numMenuOptions) % numMenuOptions;
            break;

        case sf::Keyboard::Down:
            selectedOption = (selectedOption + 1) % numMenuOptions;
            break;

        case sf::Keyboard::Enter:
            switch (selectedOption) {
            case 0: // Level Editor
                nextState = AppState::LevelEditor;
                break;
            case 1: // Game Mode
                nextState = AppState::GameMode;
                break;
            case 2: // Exit
                nextState = AppState::Exit;
                break;
            }
            break;

        case sf::Keyboard::Escape:
            nextState = AppState::Exit;
            break;

        default:
            break;
        }
    }
}

void Application::Update() {
    switch (currentState) {
    case AppState::MainMenu:
        UpdateMainMenu();
        break;

    case AppState::LevelEditor:
        levelEditor->Update(deltaTime);
        break;

    case AppState::GameMode:
        gameMode->Update(deltaTime);
        break;

    default:
        break;
    }
}

void Application::UpdateMainMenu() {
    // Update menu colors based on selection
    std::string menuOptions[3] = {
        "Level Editor",
        "Game Mode",
        "Exit"
    };

    std::string menuString = "";
    for (int i = 0; i < numMenuOptions; ++i) {
        if (i == selectedOption) {
            menuString += "> " + menuOptions[i] + " <\n\n";
        }
        else {
            menuString += "  " + menuOptions[i] + "\n\n";
        }
    }

    menuText.setString(menuString);
}

void Application::Render() {
    switch (currentState) {
    case AppState::MainMenu:
        RenderMainMenu();
        break;

    case AppState::LevelEditor:
        levelEditor->Render(window);
        break;

    case AppState::GameMode:
        gameMode->Render(window);
        break;

    default:
        break;
    }
}

void Application::RenderMainMenu() {
    // Center title
    sf::FloatRect titleBounds = titleText.getLocalBounds();
    titleText.setPosition(
        (window.getSize().x - titleBounds.width) * 0.5f,
        100.0f
    );

    // Center menu
    sf::FloatRect menuBounds = menuText.getLocalBounds();
    menuText.setPosition(
        (window.getSize().x - menuBounds.width) * 0.5f,
        300.0f
    );

    // Position instructions
    instructionsText.setPosition(50.0f, 500.0f);

    // Draw everything
    window.draw(titleText);
    window.draw(menuText);
    window.draw(instructionsText);
}

void Application::SwitchState(AppState newState) {
    std::cout << "Switching to state: " << static_cast<int>(newState) << std::endl;

    switch (newState) {
    case AppState::LevelEditor:
        // Initialize level editor if needed
        if (!levelEditor) {
            levelEditor = std::make_unique<LevelEditor>();
        }
        break;

    case AppState::GameMode:
        // Try to load the last saved map
        if (!gameMode->LoadMap("level.map")) {
            // If no saved map, create a simple test map
            MapData testMap(20, 15, 32.0f);

            // Create a simple level with walls around the border
            for (int y = 0; y < testMap.height; ++y) {
                for (int x = 0; x < testMap.width; ++x) {
                    if (x == 0 || x == testMap.width - 1 ||
                        y == 0 || y == testMap.height - 1) {
                        testMap.tiles[y][x] = TileType::SPIKES;
                    }
                    else if (y == testMap.height - 2) {
                        testMap.tiles[y][x] = TileType::BLOCK_TOP;
                    }
                    else {
                        testMap.tiles[y][x] = TileType::EMPTY;
                    }
                }
            }

            // Add some decorative elements
            testMap.tiles[testMap.height - 3][5] = TileType::TORCH;
            testMap.tiles[testMap.height - 3][testMap.width - 6] = TileType::TORCH;

            gameMode->SetMapData(testMap);
            std::cout << "Created test map for game mode" << std::endl;
        }
        break;

    case AppState::MainMenu:
        selectedOption = 0;
        break;

    default:
        break;
    }

    currentState = newState;
}

// Main function
int main() {
    try {
        Application app;

        if (!app.Initialize()) {
            std::cerr << "Failed to initialize application!" << std::endl;
            return -1;
        }

        std::cout << "Starting application..." << std::endl;
        app.Run();

        std::cout << "Application finished successfully." << std::endl;
        return 0;

    }
    catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
        return -1;
    }
    catch (...) {
        std::cerr << "Unknown exception occurred!" << std::endl;
        return -1;
    }
}