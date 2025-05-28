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
#include <string>   
#include <vector>   
#include <memory>  



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
    sf::View camera; // Vista del juego
    sf::Clock clock; // Para deltaTime

    GameState current_state;
    MapManager map_manager;
    Ray ray;
    std::vector<std::shared_ptr<Enemy>> enemies;

    int current_level;
    int max_level; // Debería cargarse desde algún archivo de configuración o calcularse
    float view_x;  // Posición X de la cámara (esquina superior izquierda de la vista)
    sf::Color background_color;

    // UI Elements
    sf::Font font;
    sf::Text ui_text;
    sf::Text menu_text;

    bool level_completed_flag; // Renombrado de level_completed para evitar confusión con algún método
    unsigned short level_complete_timer;

public:
    Game() : window(sf::VideoMode(SCREEN_WIDTH* WINDOW_SCALE, SCREEN_HEIGHT* WINDOW_SCALE), WINDOW_TITLE),
        camera(sf::FloatRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT)), // Vista con tamaño base
        current_state(GameState::MENU), current_level(1), max_level(3), // max_level podría ser dinámico
        view_x(0), background_color(sf::Color(135, 206, 235)), // Color por defecto
        level_completed_flag(false), level_complete_timer(0) {

        window.setView(camera); // Aplicar la vista a la ventana
        window.setFramerateLimit(60);

        if (!font.loadFromFile("assets/font.ttf")) {
            std::cerr << "Advertencia: No se pudo cargar la fuente 'assets/font.ttf', se usará la fuente por defecto de SFML (si existe).\n";
            // SFML podría no tener una fuente por defecto tan fácilmente, el texto podría no aparecer.
        }

        ui_text.setFont(font);
        ui_text.setCharacterSize(12); // Tamaño pequeño para UI
        ui_text.setFillColor(sf::Color::White);

        menu_text.setFont(font);
        menu_text.setCharacterSize(16); // Tamaño para menús
        menu_text.setFillColor(sf::Color::White);
        // La posición del menu_text se establece en renderMenu, etc.
    }

    void run() {
        while (window.isOpen()) {
            sf::Time elapsed = clock.restart(); // Obtener tiempo delta
            float deltaTime = elapsed.asSeconds();

            handleEvents();
            update(deltaTime); // Pasar deltaTime
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
                    else if (event.key.code == sf::Keyboard::Escape) {
                        window.close(); // Salir del juego desde el menú
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
                        current_state = GameState::MENU; // Volver al menú
                    }
                    // Aquí podrías manejar otros inputs globales del juego si los hubiera
                    break;
                default:
                    break;
                }
            }
        }
    }

    void update(float deltaTime) { // Recibe deltaTime
        switch (current_state) {
        case GameState::MENU:
            // Nada que actualizar en el menú por ahora
            break;

        case GameState::PLAYING:
            updateGame(deltaTime); // Pasar deltaTime
            break;

        case GameState::GAME_OVER:
            // Nada que actualizar en la pantalla de game over
            break;

        case GameState::LEVEL_COMPLETE:
            level_complete_timer++;
            // Podrías añadir una transición o esperar un poco antes de ir al siguiente nivel
            break;
        default:
            break;
        }
    }

    void updateGame(float deltaTime) { // Recibe y debería usar deltaTime
        // Actualizar Ray (Ray::update ya usa valores fijos, considera pasarle deltaTime)
        ray.update(view_x, map_manager); // Pasar deltaTime a Ray.update si se modifica para usarlo

        if (ray.get_dead()) {
            current_state = GameState::GAME_OVER;
            return;
        }

        // Actualizar cámara para seguir a Ray
        // view_x es la coordenada X de la esquina superior izquierda de la cámara
        view_x = ray.get_x() - SCREEN_WIDTH / 2.0f;

        // Limitar el scroll de la cámara a los bordes del mapa
        if (view_x < 0) {
            view_x = 0;
        }
        float map_pixel_width = map_manager.get_map_width() * CELL_SIZE;
        if (view_x > map_pixel_width - SCREEN_WIDTH) {
            if (map_pixel_width > SCREEN_WIDTH) { // Solo limitar si el mapa es más ancho que la pantalla
                view_x = map_pixel_width - SCREEN_WIDTH;
            }
            else { // Si el mapa es más estrecho, centrarlo o alinear a la izquierda
                view_x = 0;
            }
        }

        camera.setCenter(view_x + SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f);

        // Actualizar enemigos (pasarles deltaTime si sus updates lo usan)
        for (auto it = enemies.begin(); it != enemies.end();) {
            // (*it)->update(view_x, enemies, map_manager, ray, deltaTime); // Pasar deltaTime
            (*it)->update(view_x, enemies, map_manager, ray);

            if ((*it)->get_dead(true)) { // El 'true' es para 'deletion'
                it = enemies.erase(it);
            }
            else {
                ++it;
            }
        }

        // map_manager.update(); // ELIMINADO - No existe tal método sin argumentos
                               // Si MapManager necesitara un update (ej. para efectos animados del mapa),
                               // se debería crear ese método.

        checkLevelCompletion();
    }

    void render() {
        window.clear(background_color); // Limpiar con el color de fondo del nivel

        // Aplicar la vista de la cámara ANTES de dibujar elementos del juego
        window.setView(camera);

        switch (current_state) {
        case GameState::MENU:
            renderMenu(); // renderMenu debería dibujar en coordenadas de ventana, no de cámara
            break;
        case GameState::PLAYING:
            renderGame();
            break;
        case GameState::GAME_OVER:
            renderGameOver(); // También en coordenadas de ventana
            break;
        case GameState::LEVEL_COMPLETE:
            renderLevelComplete(); // También en coordenadas de ventana
            break;
        default:
            break;
        }

        window.display();
    }

    void renderMenu() {
        // Para dibujar UI/menú, es mejor usar la vista por defecto de la ventana
        window.setView(window.getDefaultView());

        menu_text.setString("RAY'S ADVENTURE\n\nPress ENTER to start\nPress ESC to exit\n\nCurrent Level: " + std::to_string(current_level));
        // Centrar el texto (aproximado)
        sf::FloatRect textRect = menu_text.getLocalBounds();
        menu_text.setOrigin(textRect.left + textRect.width / 2.0f, textRect.top + textRect.height / 2.0f);
        menu_text.setPosition(window.getSize().x / 2.0f, window.getSize().y / 2.0f);
        window.draw(menu_text);
    }

    void renderGame() {
        // La vista de la cámara ya debería estar establecida por render() o updateGame()
        // window.setView(camera); // Ya se hizo en render()

        map_manager.draw_map(true, current_level, view_x, window); // 'true' y 'current_level' no se usan en la impl. de MapManager::draw_map

        ray.draw(window); // Ray se dibuja en coordenadas relativas a la cámara (view_x)
        ray.draw_leafs(view_x, window); // Leafs también

        for (auto& enemy : enemies) {
            enemy->draw(view_x, window); // Enemigos también
        }

        renderUI(); // Dibujar UI encima de todo
    }

    void renderUI() {
        // Cambiar a la vista por defecto para dibujar la UI en posiciones fijas de la pantalla
        window.setView(window.getDefaultView());

        ui_text.setPosition(10 * WINDOW_SCALE, 10 * WINDOW_SCALE); // Ajustar por escala de ventana si es necesario
        ui_text.setString("Lives: " + std::to_string(ray.get_lifes()));
        window.draw(ui_text);

        ui_text.setPosition(10 * WINDOW_SCALE, (10 + 15) * WINDOW_SCALE); // Un poco más abajo
        ui_text.setString("Level: " + std::to_string(current_level));
        window.draw(ui_text);

        // Al final de renderUI, no es necesario restaurar la vista de la cámara aquí,
        // ya que render() o renderGame() lo harán al principio de su ejecución.
    }

    void renderGameOver() {
        window.setView(window.getDefaultView()); // Usar vista de ventana
        menu_text.setString("GAME OVER\n\nPress R to restart\nPress ESC for menu");
        sf::FloatRect textRect = menu_text.getLocalBounds();
        menu_text.setOrigin(textRect.left + textRect.width / 2.0f, textRect.top + textRect.height / 2.0f);
        menu_text.setPosition(window.getSize().x / 2.0f, window.getSize().y / 2.0f);
        window.draw(menu_text);
    }

    void renderLevelComplete() {
        window.setView(window.getDefaultView()); // Usar vista de ventana
        menu_text.setString("LEVEL COMPLETE!\n\nPress ENTER to continue");
        sf::FloatRect textRect = menu_text.getLocalBounds();
        menu_text.setOrigin(textRect.left + textRect.width / 2.0f, textRect.top + textRect.height / 2.0f);
        menu_text.setPosition(window.getSize().x / 2.0f, window.getSize().y / 2.0f);
        window.draw(menu_text);
    }

    void startGame() {
        current_level = 1;
        loadLevel(current_level);
        current_state = GameState::PLAYING;
    }

    void restartGame() {
        // No es necesario limpiar enemigos aquí si loadLevel ya lo hace.
        loadLevel(current_level); // loadLevel resetea a Ray y carga todo
        current_state = GameState::PLAYING;
    }

    void nextLevel() {
        current_level++;
        if (current_level > max_level) {
            current_state = GameState::MENU; // Volver al menú si se completan todos los niveles
            current_level = 1; // O mostrar pantalla de "Juego Completado"
        }
        else {
            loadLevel(current_level);
            current_state = GameState::PLAYING;
        }
    }

    void loadLevel(int level_num) {
        enemies.clear(); // Limpiar enemigos del nivel anterior
        ray.reset();     // Resetear estado de Ray

        // Cargar la estructura del mapa desde el archivo de texto
        map_manager.load_map_from_text(level_num);

        // Cargar entidades (Ray, enemigos) y configuración del nivel
        loadLevelData(level_num); // Esta función ahora se encarga de Ray y enemigos

        level_completed_flag = false;
        level_complete_timer = 0;

        // Establecer la posición inicial de la vista de la cámara.
        // Ray.set_position es llamado en loadLevelData. view_x se ajustará en updateGame.
        // Forzar un cálculo inicial de view_x podría ser bueno:
        view_x = ray.get_x() - SCREEN_WIDTH / 2.0f;
        if (view_x < 0) view_x = 0;
        float map_pixel_width = map_manager.get_map_width() * CELL_SIZE;
        if (view_x > map_pixel_width - SCREEN_WIDTH) {
            if (map_pixel_width > SCREEN_WIDTH) view_x = map_pixel_width - SCREEN_WIDTH;
            else view_x = 0;
        }
        camera.setCenter(view_x + SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f);


        std::cout << "Loaded level " << level_num << std::endl;
    }

    void loadLevelData(int level_num) {
        std::string filename = "maps/level" + std::to_string(level_num) + ".txt";
        std::ifstream file(filename);

        if (!file.is_open()) {
            std::cerr << "Error en loadLevelData: No se pudo abrir " << filename << std::endl;
            // Valores por defecto si el archivo no se encuentra
            ray.set_position(2 * CELL_SIZE, 2 * CELL_SIZE); // Posición segura
            background_color = sf::Color(100, 100, 100); // Gris por defecto
            return;
        }

        std::string line;
        int r = 0; // row index
        bool ray_pos_set = false;

        while (std::getline(file, line)) {
            for (int c = 0; c < static_cast<int>(line.length()); ++c) { // c es column index
                char entity_char = line[c];
                float world_x = c * CELL_SIZE + CELL_SIZE / 2.0f; // Centrar entidad en la celda
                float world_y = r * CELL_SIZE + CELL_SIZE / 2.0f; // Centrar entidad en la celda
                // O ajustar según el origen del sprite.
                // La lógica original usaba esquina sup-izq: c * 16.0f

                switch (entity_char) {
                case 'P': // Player start position
                    ray.set_position(c * CELL_SIZE, r * CELL_SIZE); // Esquina superior izquierda de la celda
                    ray_pos_set = true;
                    break;
                case 'A': // Infected Ant (normal)
                    enemies.push_back(std::make_shared<Infected_Ant>(false, c * CELL_SIZE, r * CELL_SIZE));
                    break;
                case 'a': // Infected Ant (aggressive)
                    enemies.push_back(std::make_shared<Infected_Ant>(true, c * CELL_SIZE, r * CELL_SIZE));
                    break;
                case 'M': // ReyHongo (normal)
                    enemies.push_back(std::make_shared<ReyHongo>(false, c * CELL_SIZE, r * CELL_SIZE));
                    break;
                case 'm': // ReyHongo (aggressive)
                    enemies.push_back(std::make_shared<ReyHongo>(true, c * CELL_SIZE, r * CELL_SIZE));
                    break;
                    // Otros tipos de entidades podrían ir aquí
                default:
                    // Celdas del mapa ya cargadas por map_manager.load_map_from_text
                    break;
                }
            }
            r++;
        }
        file.close();

        if (!ray_pos_set) { // Si no se encontró 'P' en el archivo de nivel
            ray.set_position(2 * CELL_SIZE, 2 * CELL_SIZE); // Posición por defecto
            std::cout << "Advertencia: No se encontró la posición de inicio 'P' para Ray en el nivel " << level_num
                << ". Usando posición por defecto." << std::endl;
        }

        // Configurar color de fondo basado en el nivel
        switch (level_num) {
        case 1: background_color = sf::Color(135, 206, 235); break; // Sky blue
        case 2: background_color = sf::Color(25, 25, 112); break;   // Midnight blue
        case 3: background_color = sf::Color(139, 69, 19); break;  // Saddle brown
        default: background_color = sf::Color(120, 120, 120); break; // Gris por defecto
        }
    }

    void checkLevelCompletion() {
        // Condición de ejemplo: llegar cerca del borde derecho del mapa
        // Usar map_manager.get_map_width() para obtener el ancho del mapa en celdas
        if (ray.get_x() > (map_manager.get_map_width() - 2) * CELL_SIZE) { // -2 celdas del borde
            if (!level_completed_flag) {
                level_completed_flag = true;
                level_complete_timer = 0;
                current_state = GameState::LEVEL_COMPLETE;
                std::cout << "Level " << current_level << " completed!" << std::endl;
            }
        }
    }
}; // Fin de la clase Game

int main() {
    // Es buena práctica tener un try-catch para excepciones no esperadas
    try {
        Game game;
        game.run();
    }
    catch (const std::exception& e) {
        std::cerr << "Error fatal: " << e.what() << std::endl;
        // Podrías añadir aquí una pausa o un input para que el usuario vea el error en consola
        // antes de que se cierre.
#ifdef _WIN32
        system("pause"); // Solo para Windows, para desarrollo
#endif
        return EXIT_FAILURE; // Usar EXIT_FAILURE de <cstdlib>
    }
    catch (...) {
        std::cerr << "Error fatal desconocido." << std::endl;
#ifdef _WIN32
        system("pause");
#endif
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS; // Usar EXIT_SUCCESS de <cstdlib>
}