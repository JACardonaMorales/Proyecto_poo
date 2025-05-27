#include "stdafx.h"
#include "Ray.h"
#include "MapManager.h"
#include "Enemy.h"
#include "Global.h"

int main() {
    // Create the main window
    sf::RenderWindow window(sf::VideoMode(800, 600), "Ray Game");
    window.setFramerateLimit(60);

    // Game objects
    Ray ray;
    MapManager map_manager;
    std::vector<std::shared_ptr<Enemy>> enemies;

    // Game variables
    float view_x = 0.0f;
    int current_level = 1;
    bool level_finish = false;
    sf::Color bg_color = sf::Color::Blue;

    // Load initial level
    convert_sketch(current_level, level_finish, enemies, bg_color, map_manager, ray);

    // Game clock
    sf::Clock clock;

    // Game loop
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }

            // Reset game with R key
            if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::R) {
                    ray.reset();
                    enemies.clear();
                    convert_sketch(current_level, level_finish, enemies, bg_color, map_manager, ray);
                }

                // Change level with number keys
                if (event.key.code >= sf::Keyboard::Num1 && event.key.code <= sf::Keyboard::Num9) {
                    current_level = event.key.code - sf::Keyboard::Num0;
                    ray.reset();
                    enemies.clear();
                    convert_sketch(current_level, level_finish, enemies, bg_color, map_manager, ray);
                }
            }
        }

        // Update game logic
        float dt = clock.restart().asSeconds();

        // Update Ray
        ray.update(view_x, map_manager);

        // Update camera to follow Ray
        float target_view_x = ray.get_x() - 400; // Center Ray on screen
        if (target_view_x < 0) target_view_x = 0;
        view_x += (target_view_x - view_x) * 0.1f; // Smooth camera

        // Update enemies
        for (auto& enemy : enemies) {
            if (enemy && !enemy->get_dead()) {
                enemy->update(view_x, enemies, map_manager, ray);
            }
        }

        // Remove dead enemies
        enemies.erase(
            std::remove_if(enemies.begin(), enemies.end(),
                [](const std::shared_ptr<Enemy>& enemy) {
                    return enemy->get_dead(true);
                }),
            enemies.end());

        // Update map manager
        map_manager.update();

        // Render everything
        window.clear(bg_color);

        // Draw map
        map_manager.draw_map(true, current_level, view_x, window);

        // Draw enemies
        for (auto& enemy : enemies) {
            if (enemy) {
                enemy->draw(view_x, window);
            }
        }

        // Draw Ray
        ray.draw(window);
        ray.draw_leafs(view_x, window);

        // Draw UI
        sf::Font font;
        if (font.loadFromFile("arial.ttf")) {
            sf::Text lives_text;
            lives_text.setFont(font);
            lives_text.setString("Lives: " + std::to_string(ray.get_lifes()));
            lives_text.setCharacterSize(24);
            lives_text.setFillColor(sf::Color::White);
            lives_text.setPosition(10, 10);
            window.draw(lives_text);

            sf::Text level_text;
            level_text.setFont(font);
            level_text.setString("Level: " + std::to_string(current_level));
            level_text.setCharacterSize(24);
            level_text.setFillColor(sf::Color::White);
            level_text.setPosition(10, 40);
            window.draw(level_text);

            if (ray.get_dead()) {
                sf::Text game_over;
                game_over.setFont(font);
                game_over.setString("GAME OVER - Press R to restart");
                game_over.setCharacterSize(36);
                game_over.setFillColor(sf::Color::Red);
                game_over.setPosition(200, 300);
                window.draw(game_over);
            }
        }

        window.display();
    }

    std::cout << "Game terminated successfully." << std::endl;
    return 0;
}