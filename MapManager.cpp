#include "Global.h"
#include "MapManager.h"
#include <fstream>
#include <sstream>

MapManager::MapManager()
{

    // Cargar texturas
    if (!map_texture.loadFromFile("assets/World/bases.png")) {
        std::cout << "Error: No se pudo cargar tileset.png" << std::endl;
    }

    if (!spikes_texture.loadFromFile("assets/World/spikes.png")) {
        std::cout << "Error: No se pudo cargar spikes.png" << std::endl;
    }

    if (!door_texture.loadFromFile("assets/World/door.png")) {
        std::cout << "Error: No se pudo cargar door.png" << std::endl;
    }

    cell_sprite.setTexture(map_texture);
}

unsigned short MapManager::get_map_sketch_height() const {
    return map_sketch.getSize().y;
}

unsigned short MapManager::get_map_sketch_width() const {
    return map_sketch.getSize().x;
}

void MapManager::load_map_from_text(int level) {
    std::string filename = "maps/level" + std::to_string(level) + ".txt";
    std::ifstream file(filename);

    if (!file.is_open()) {
        std::cout << "Error: Could not open " << filename << std::endl;
        // Create a default map
        map.assign(15, std::vector<Cell>(20, Cell::Empty));
        // Add some ground
        for (int i = 0; i < 20; i++) {
            map[14][i] = Cell::Wall;
        }
        return;
    }

    // Clear existing map
    map.clear();

    std::string line;
    int row = 0;

    while (std::getline(file, line) && row < 15) { // Max 15 rows
        std::vector<Cell> map_row;

        for (int col = 0; col < 20; col++) { // Max 20 columns
            Cell cell = Cell::Empty;

            if (col < line.length()) {
                char c = line[col];

                switch (c) {
                case '#': cell = Cell::Wall; break;
                case 'G': cell = Cell::Grass; break;
                case 'L': cell = Cell::Light; break;
                case 'H': cell = Cell::Life; break;
                case 'E': cell = Cell::Entrance; break;
                case 'R': cell = Cell::Prock; break;
                case 'S': cell = Cell::Spikes; break;
                case 'D': cell = Cell::Door; break;
                case ' ':
                case '.':
                case 'P': // Player position
                case 'A': // Ant position
                case 'a': // Aggressive ant
                case 'M': // ReyHongo
                case 'm': // Aggressive ReyHongo
                default:
                    cell = Cell::Empty;
                    break;
                }
            }

            map_row.push_back(cell);
        }

        map.push_back(map_row);
        row++;
    }

    file.close();

    // Ensure map has minimum size
    while (map.size() < 15) {
        map.push_back(std::vector<Cell>(20, Cell::Empty));
    }

    std::cout << "Loaded map from " << filename << std::endl;
}

// Verificar que estos métodos existan (si no, agregar):
unsigned short MapManager::get_map_width() const {
    return map.empty() ? 0 : map[0].size();
}

unsigned short MapManager::get_map_height() const {
    return map.size();
}

void MapManager::draw_map(bool draw_background, int level, float view_x, sf::RenderWindow& window) {
    const int CELL_SIZE = 16;
    int start_x = static_cast<int>(view_x / CELL_SIZE);
    int end_x = start_x + (window.getSize().x / CELL_SIZE) + 2;

    for (int y = 0; y < static_cast<int>(map.size()); y++) {
        for (int x = start_x; x < end_x && x < static_cast<int>(map[y].size()); x++) {
            if (x < 0) continue;

            Cell cell = map[y][x];
            float pos_x = x * CELL_SIZE - view_x;
            float pos_y = y * CELL_SIZE;

            if (cell != Cell::Empty) {
                if (cell == Cell::Spikes) {
                    // Dibujar pinchos con textura específica
                    sf::Sprite spikes_sprite;
                    spikes_sprite.setTexture(spikes_texture);
                    spikes_sprite.setPosition(pos_x, pos_y);
                    window.draw(spikes_sprite);
                }
                else if (cell == Cell::Door) {
                    // Dibujar puerta con textura específica
                    sf::Sprite door_sprite;
                    door_sprite.setTexture(door_texture);
                    door_sprite.setPosition(pos_x, pos_y);
                    window.draw(door_sprite);
                }
                else {
                    // Dibujar celdas normales del tileset
                    int tex_x = static_cast<int>(cell) * CELL_SIZE;
                    cell_sprite.setTextureRect(sf::IntRect(tex_x, 0, CELL_SIZE, CELL_SIZE));
                    cell_sprite.setPosition(pos_x, pos_y);
                    window.draw(cell_sprite);
                }
            }
        }
    }

}

void MapManager::set_map_cell(int x, int y, Cell cell) {
    if (y >= 0 && y < static_cast<int>(map.size()) &&
        x >= 0 && x < static_cast<int>(map[y].size())) {
        map[y][x] = cell;
    }
}

void MapManager::set_map_size(unsigned int new_size) {
    map.resize(new_size);
    for (auto& row : map) {
        row.resize(new_size, Cell::Empty);
    }
}


void MapManager::update_map_sketch(int current_level) {
    std::string filename = "maps/level_" + std::to_string(current_level) + ".png";
    if (!map_sketch.loadFromFile(filename)) {
        std::cout << "Advertencia: No se pudo cargar " << filename << std::endl;
    }
}

std::vector<unsigned char> MapManager::map_collision(const std::vector<Cell>& check_cells,
    const sf::FloatRect& hitbox) {
    std::vector<unsigned char> collisions;
    const int CELL_SIZE = 16;

    int left = static_cast<int>(hitbox.left / CELL_SIZE);
    int right = static_cast<int>((hitbox.left + hitbox.width) / CELL_SIZE);
    int top = static_cast<int>(hitbox.top / CELL_SIZE);
    int bottom = static_cast<int>((hitbox.top + hitbox.height) / CELL_SIZE);

    for (int y = top; y <= bottom; y++) {
        for (int x = left; x <= right; x++) {
            if (y >= 0 && y < static_cast<int>(map.size()) &&
                x >= 0 && x < static_cast<int>(map[y].size())) {

                Cell cell = map[y][x];
                for (size_t i = 0; i < check_cells.size(); i++) {
                    if (cell == check_cells[i]) {
                        collisions.push_back(static_cast<unsigned char>(i));
                        break;
                    }
                }
            }
        }
    }

    return collisions;
}

std::vector<unsigned char> MapManager::map_collision(const std::vector<Cell>& check_cells,
    const std::vector<Cell>& collision_cells,
    const sf::FloatRect& hitbox) {
    return map_collision(check_cells, hitbox);
}

sf::Color MapManager::get_map_sketch_pixel(int x, int y) const {
    if (x >= 0 && x < static_cast<int>(map_sketch.getSize().x) &&
        y >= 0 && y < static_cast<int>(map_sketch.getSize().y)) {
        return map_sketch.getPixel(x, y);
    }
    return sf::Color::Black;
}
