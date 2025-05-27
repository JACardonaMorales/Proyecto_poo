#include "Global.h"
#include "MapManager.h"
#include "Ray.h"
#include "Enemy.h"
#include "Infected_Ant.h"
#include "ReyHongo.h"
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <iostream>
#include <fstream>
#include <sstream>

// Game constants
const unsigned int WINDOW_WIDTH = 320;
const unsigned int WINDOW_HEIGHT = 240;
const unsigned int WINDOW_SCALE = 3;
const std::string WINDOW_TITLE = "Ray's Adventure";

// Game states
enum class GameState {
    MENU,
    PLAYING,
    GAME_OVER,
    LEVEL_COMPLETE
};

class Game {
private:
    sf::RenderWindow window;
    sf::View camera;
    sf::Clock clock;

    GameState current_state;
    MapManager map_manager;
    Ray ray;
    std::vector<std::shared_ptr<Enemy>> enemies;

    int current_level;
    int max_level;
    float view_x;
    sf::Color background_color;

    // UI Elements
    sf::Font font;
    sf::Text ui_text;
    sf::Text menu_text;

    bool level_completed;
    unsigned short level_complete_timer;

public:
    Game() : window(sf::VideoMode(WINDOW_WIDTH* WINDOW_SCALE, WINDOW_HEIGHT* WINDOW_SCALE), WINDOW_TITLE),
        camera(sf::FloatRect(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT)),
        current_state(GameState::MENU), current_level(1), max_level(3),
        view_x(0), background_color(sf::Color::Blue), level_completed(false),
        level_complete_timer(0) {

        window.setView(camera);
        window.setFramerateLimit(60);

        // Initialize font (you'll need to provide a font file)
        if (!font.loadFromFile("assets/font.ttf")) {
            // If no font file, use default font
            std::cout << "Warning: Could not load font, using default\n";
        }

        ui_text.setFont(font);
        ui_text.setCharacterSize(12);
        ui_text.setFillColor(sf::Color::White);

        menu_text.setFont(font);
        menu_text.setCharacterSize(16);
        menu_text.setFillColor(sf::Color::White);
        menu_text.setPosition(50, 100);
    }

    void run() {
        while (window.isOpen()) {
            float deltaTime = clock.restart().asSeconds();

            handleEvents();
            update(deltaTime);
            render();
        }
    }

private:
    void handleEvents() {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }

            if (event.type == sf::Event::KeyPressed) {
                switch (current_state) {
                case GameState::MENU:
                    if (event.key.code == sf::Keyboard::Enter) {
                        startGame();
                    }
                    break;

                case GameState::GAME_OVER:
                    if (event.key.code == sf::Keyboard::R) {
                        restartGame();
                    }
                    else if (event.key.code == sf::Keyboard::Escape) {
                        current_state = GameState::MENU;
                    }
                    break;

                case GameState::LEVEL_COMPLETE:
                    if (event.key.code == sf::Keyboard::Enter) {
                        nextLevel();
                    }
                    break;

                case GameState::PLAYING:
                    if (event.key.code == sf::Keyboard::Escape) {
                        current_state = GameState::MENU;
                    }
                    break;
                }
            }
        }
    }

    void update(float deltaTime) {
        switch (current_state) {
        case GameState::MENU:
            // Menu doesn't need updates
            break;

        case GameState::PLAYING:
            updateGame(deltaTime);
            break;

        case GameState::GAME_OVER:
            // Game over screen doesn't need updates
            break;

        case GameState::LEVEL_COMPLETE:
            level_complete_timer++;
            break;
        }
    }

    void updateGame(float deltaTime) {
        // Update Ray
        ray.update(view_x, map_manager);

        // Check if Ray died
        if (ray.get_dead()) {
            current_state = GameState::GAME_OVER;
            return;
        }

        // Update camera to follow Ray
        view_x = ray.get_x() - WINDOW_WIDTH / 2;
        if (view_x < 0) view_x = 0;
        if (view_x > map_manager.get_map_width() * 16 - WINDOW_WIDTH) {
            view_x = map_manager.get_map_width() * 16 - WINDOW_WIDTH;
        }

        // Update enemies
        for (auto it = enemies.begin(); it != enemies.end();) {
            (*it)->update(view_x, enemies, map_manager, ray);

            if ((*it)->get_dead(true)) {
                it = enemies.erase(it);
            }
            else {
                ++it;
            }
        }

        // Update map manager
        map_manager.update();

        // Check level completion (you can customize this condition)
        checkLevelCompletion();
    }

    void render() {
        window.clear(background_color);

        switch (current_state) {
        case GameState::MENU:
            renderMenu();
            break;

        case GameState::PLAYING:
            renderGame();
            break;

        case GameState::GAME_OVER:
            renderGameOver();
            break;

        case GameState::LEVEL_COMPLETE:
            renderLevelComplete();
            break;
        }

        window.display();
    }

    void renderMenu() {
        menu_text.setString("RAY'S ADVENTURE\n\nPress ENTER to start\n\nLevel: " + std::to_string(current_level));
        window.draw(menu_text);
    }

    void renderGame() {
        // Update camera position
        camera.setCenter(view_x + WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2);
        window.setView(camera);

        // Draw map
        map_manager.draw_map(true, current_level, view_x, window);

        // Draw Ray
        ray.draw(window);

        // Draw life objects
        ray.draw_leafs(view_x, window);

        // Draw enemies
        for (auto& enemy : enemies) {
            enemy->draw(view_x, window);
        }

        // Draw UI
        renderUI();
    }

    void renderUI() {
        // Reset view for UI
        sf::View ui_view(sf::FloatRect(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT));
        window.setView(ui_view);

        // Draw lives
        ui_text.setPosition(10, 10);
        ui_text.setString("Lives: " + std::to_string(ray.get_lifes()));
        window.draw(ui_text);

        // Draw level
        ui_text.setPosition(10, 25);
        ui_text.setString("Level: " + std::to_string(current_level));
        window.draw(ui_text);

        // Restore game view
        camera.setCenter(view_x + WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2);
        window.setView(camera);
    }

    void renderGameOver() {
        menu_text.setString("GAME OVER\n\nPress R to restart\nPress ESC for menu");
        window.draw(menu_text);
    }

    void renderLevelComplete() {
        menu_text.setString("LEVEL COMPLETE!\n\nPress ENTER to continue");
        window.draw(menu_text);
    }

    void startGame() {
        current_level = 1;
        loadLevel(current_level);
        current_state = GameState::PLAYING;
    }

    void restartGame() {
        ray.reset();
        enemies.clear();
        loadLevel(current_level);
        current_state = GameState::PLAYING;
    }

    void nextLevel() {
        current_level++;
        if (current_level > max_level) {
            current_state = GameState::MENU;
            current_level = 1;
        }
        else {
            loadLevel(current_level);
            current_state = GameState::PLAYING;
        }
    }

    void loadLevel(int level) {
        // Clear existing enemies
        enemies.clear();

        // Reset Ray
        ray.reset();

        // Load map from text file
        map_manager.load_map_from_text(level);

        // Load level data from text file
        loadLevelData(level);

        // Reset level completion
        level_completed = false;
        level_complete_timer = 0;

        // Set view position
        view_x = 0;

        std::cout << "Loaded level " << level << std::endl;
    }

    void loadLevelData(int level) {
        std::string filename = "maps/level" + std::to_string(level) + ".txt";
        std::ifstream file(filename);

        if (!file.is_open()) {
            std::cout << "Warning: Could not open " << filename << std::endl;
            // Set default values
            ray.set_position(32, 32);
            background_color = sf::Color(135, 206, 235); // Sky blue
            return;
        }

        std::string line;
        int row = 0;
        float ray_x = 32, ray_y = 32;

        while (std::getline(file, line) && row < 15) { // Assuming 15 rows max
            for (int col = 0; col < line.length() && col < 20; col++) { // Assuming 20 columns max
                char cell = line[col];
                float x = col * 16.0f;
                float y = row * 16.0f;

                switch (cell) {
                case 'P': // Player start position
                    ray_x = x;
                    ray_y = y;
                    break;

                case 'A': // Infected Ant (normal)
                    enemies.push_back(std::make_shared<Infected_Ant>(false, x, y));
                    break;

                case 'a': // Infected Ant (aggressive)
                    enemies.push_back(std::make_shared<Infected_Ant>(true, x, y));
                    break;

                case 'M': // ReyHongo (normal)
                    enemies.push_back(std::make_shared<ReyHongo>(false, x, y));
                    break;

                case 'm': // ReyHongo (aggressive)
                    enemies.push_back(std::make_shared<ReyHongo>(true, x, y));
                    break;

                    // Map cells are handled by MapManager
                default:
                    break;
                }
            }
            row++;
        }

        file.close();

        // Set Ray's starting position
        ray.set_position(ray_x, ray_y);

        // Set background color based on level
        switch (level) {
        case 1:
            background_color = sf::Color(135, 206, 235); // Sky blue
            break;
        case 2:
            background_color = sf::Color(25, 25, 112); // Midnight blue
            break;
        case 3:
            background_color = sf::Color(139, 69, 19); // Saddle brown
            break;
        default:
            background_color = sf::Color(135, 206, 235);
            break;
        }
    }

    void checkLevelCompletion() {
        // Example completion condition: reach the right edge of the map
        if (ray.get_x() > (map_manager.get_map_width() - 2) * 16) {
            if (!level_completed) {
                level_completed = true;
                level_complete_timer = 0;
                current_state = GameState::LEVEL_COMPLETE;
            }
        }
    }
};

int main() {
    try {
        Game game;
        game.run();
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return -1;
    }

    return 0;
}