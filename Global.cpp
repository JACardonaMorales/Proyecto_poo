
#include "Global.h"
#include "MapManager.h"
#include "Ray.h"
#include "Enemy.h"
#include "Infected_Ant.h"
#include "ReyHongo.h"
#include <memory>
#include <fstream>

sf::Color convert_sketch_from_text(int level, bool& level_finish,
    std::vector<std::shared_ptr<Enemy>>& enemies,
    sf::Color bg_color, MapManager& map_manager, Ray& ray) {

    // Cargar el mapa desde archivo de texto
    map_manager.load_map_from_text(level);

    // Limpiar enemigos existentes
    enemies.clear();

    // Buscar elementos especiales en el mapa y crear enemigos
    std::string filename = "maps/level_" + std::to_string(level) + ".txt";
    std::ifstream file(filename);

    if (!file.is_open()) {
        std::cout << "Error: No se pudo abrir " << filename << " para buscar enemigos" << std::endl;
        return bg_color;
    }

    std::string line;
    int y = 0;

    while (std::getline(file, line)) {
        for (int x = 0; x < static_cast<int>(line.length()); x++) {
            char c = line[x];
            float world_x = x * 16.0f;
            float world_y = y * 16.0f;

            switch (c) {
            case 'S': // Spawn del jugador (Entrance)
                ray.set_position(world_x, world_y);
                break;

            case 'A': // Hormiga infectada normal
                enemies.push_back(std::make_shared<Infected_Ant>(false, world_x, world_y));
                break;

            case 'a': // Hormiga infectada atacante
                enemies.push_back(std::make_shared<Infected_Ant>(true, world_x, world_y));
                break;

            case 'M': // ReyHongo normal
                enemies.push_back(std::make_shared<ReyHongo>(false, world_x, world_y));
                break;

            case 'm': // ReyHongo atacante
                enemies.push_back(std::make_shared<ReyHongo>(true, world_x, world_y));
                break;
            }
        }
        y++;
    }
    file.close();

    std::cout << "Enemigos cargados: " << enemies.size() << std::endl;
    return bg_color;
}

// Función original para compatibilidad con imágenes
sf::Color convert_sketch(int level, bool level_finish, std::vector<std::shared_ptr<Enemy>>& enemies,
    sf::Color bg_color, MapManager& map_manager, Ray& ray) {

    map_manager.update_map_sketch(level);

    unsigned int width = map_manager.get_map_sketch_width();
    unsigned int height = map_manager.get_map_sketch_height();

    map_manager.set_map_size(height);

    // Clear existing enemies
    enemies.clear();

    // Convert sketch pixels to map cells
    for (unsigned int y = 0; y < height; y++) {
        for (unsigned int x = 0; x < width; x++) {
            sf::Color pixel = map_manager.get_map_sketch_pixel(x, y);
            Cell cell = Cell::Empty;

            // Convert colors to cell types
            if (pixel == sf::Color::Black) {
                cell = Cell::Wall;
            }
            else if (pixel == sf::Color::Green) {
                cell = Cell::Grass;
            }
            else if (pixel == sf::Color::Blue) {
                cell = Cell::Light;
            }
            else if (pixel == sf::Color::Red) {
                cell = Cell::Life;
            }
            else if (pixel == sf::Color::Yellow) {
                cell = Cell::Entrance;
                // Set Ray's starting position
                ray.set_position(x * 16, y * 16);
            }
            else if (pixel == sf::Color(128, 128, 128)) {
                cell = Cell::Prock;
            }
            else if (pixel == sf::Color(255, 0, 255)) {
                // Magenta = Infected Ant
                cell = Cell::Empty;
                enemies.push_back(std::make_shared<Infected_Ant>(false, x * 16, y * 16));
            }
            else if (pixel == sf::Color(0, 255, 255)) {
                // Cyan = Attacking Infected Ant
                cell = Cell::Empty;
                enemies.push_back(std::make_shared<Infected_Ant>(true, x * 16, y * 16));
            }
            else if (pixel == sf::Color(255, 128, 0)) {
                // Orange = ReyHongo
                cell = Cell::Empty;
                enemies.push_back(std::make_shared<ReyHongo>(false, x * 16, y * 16));
            }
            else if (pixel == sf::Color(255, 0, 128)) {
                // Pink = Attacking ReyHongo
                cell = Cell::Empty;
                enemies.push_back(std::make_shared<ReyHongo>(true, x * 16, y * 16));
            }

            map_manager.set_map_cell(x, y, cell);
        }
    }

    return bg_color; // Return the background color
}

void draw_map(float view_x, const sf::Image& map_sketch, sf::RenderWindow& window,
    const sf::Texture& map_texture, const std::vector<std::vector<Cell>>& map) {

    sf::Sprite cell_sprite;
    cell_sprite.setTexture(map_texture);

    const int CELL_SIZE = 16;
    int start_x = static_cast<int>(view_x / CELL_SIZE);
    int end_x = start_x + (window.getSize().x / CELL_SIZE) + 2;

    for (int y = 0; y < static_cast<int>(map.size()); y++) {
        for (int x = start_x; x < end_x && x < static_cast<int>(map[y].size()); x++) {
            if (x < 0) continue;

            Cell cell = map[y][x];
            if (cell != Cell::Empty) {
                // Set texture rectangle based on cell type
                int tex_x = static_cast<int>(cell) * CELL_SIZE;
                cell_sprite.setTextureRect(sf::IntRect(tex_x, 0, CELL_SIZE, CELL_SIZE));
                cell_sprite.setPosition(x * CELL_SIZE - view_x, y * CELL_SIZE);
                window.draw(cell_sprite);
            }
        }
    }
}

unsigned char map_collision(float x, float y, const std::vector<Cell>& check_cells,
    const std::vector<std::vector<Cell>>& map) {

    const int CELL_SIZE = 16;
    int map_x = static_cast<int>(x / CELL_SIZE);
    int map_y = static_cast<int>(y / CELL_SIZE);

    if (map_y >= 0 && map_y < static_cast<int>(map.size()) &&
        map_x >= 0 && x < static_cast<int>(map[map_y].size())) {

        Cell cell = map[map_y][map_x];

        for (size_t i = 0; i < check_cells.size(); i++) {
            if (cell == check_cells[i]) {
                return static_cast<unsigned char>(i);
            }
        }
    }

    return 255; // No collision
}