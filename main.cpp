#include "stdafx.h"
#include "Game.h"

int main()
{
    // Window settings
    const unsigned int WINDOW_WIDTH = 1200;
    const unsigned int WINDOW_HEIGHT = 800;
    const std::string WINDOW_TITLE = "Dead Paradise";

    // Create window
    sf::RenderWindow window(
        sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT),
        WINDOW_TITLE,
        sf::Style::Titlebar | sf::Style::Close
    );

    window.setFramerateLimit(60);
    window.setVerticalSyncEnabled(true);

    // Create renderer
    Renderer renderer(window);

    // Initialize game
    init(window);

    // Game timing
    sf::Clock deltaClock;
    sf::Clock fpsClock;
    int frameCount = 0;

    std::cout << "Starting game loop..." << std::endl;
    std::cout << "Controls:" << std::endl;
    std::cout << "- TAB: Toggle Level Editor" << std::endl;
    std::cout << "- WASD/Arrow Keys: Move (Game) / Move Camera (Editor)" << std::endl;
    std::cout << "- Space: Jump (Game)" << std::endl;
    std::cout << "- F5: Quick Save" << std::endl;
    std::cout << "- F9: Quick Load" << std::endl;
    std::cout << "- ESC: Exit" << std::endl;
    std::cout << std::endl;
    std::cout << "Level Editor Controls:" << std::endl;
    std::cout << "- 1-5: Select basic tiles (Wall, Platform, Spikes, Torch, Door)" << std::endl;
    std::cout << "- Q,W,E,A,S,D,Z,X,C: Select tileset blocks" << std::endl;
    std::cout << "- Left Click: Place tile" << std::endl;
    std::cout << "- Right Click: Erase tile" << std::endl;
    std::cout << "- G: Toggle grid" << std::endl;
    std::cout << "- F1: Save level, F2: Load level, F3: Export as image" << std::endl;

    // Main game loop
    while (window.isOpen())
    {
        // Calculate delta time
        float deltaTime = deltaClock.restart().asSeconds();

        // Handle events
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
            {
                window.close();
            }

            // Handle ESC key to close
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)
            {
                if (!showLevelEditor)
                {
                    window.close();
                }
            }

            // Handle game input
            HandleInput(event, window);
        }

        // Update game logic
        Update(deltaTime);

        // Set camera view
        sf::View gameView = camera.getView(sf::Vector2f(WINDOW_WIDTH, WINDOW_HEIGHT));
        window.setView(gameView);

        // Clear window
        window.clear(sf::Color(0, 0, 0)); // Dark background

        // Render game objects
        Render(renderer);

        // Reset view for UI
        window.setView(window.getDefaultView());

        // Render UI
        RenderUI(window);

        // Display frame
        window.display();

        // Calculate and display FPS every second
        frameCount++;
        if (fpsClock.getElapsedTime().asSeconds() >= 1.0f)
        {
            float fps = frameCount / fpsClock.restart().asSeconds();
            std::string newTitle = WINDOW_TITLE + " - FPS: " + std::to_string((int)fps);
            window.setTitle(newTitle);
            frameCount = 0;
        }
    }

    std::cout << "Game ended. Goodbye!" << std::endl;
    return 0;
}