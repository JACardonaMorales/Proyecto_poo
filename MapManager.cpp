#include "Global.h"
#include "MapManager.h"
#include <fstream>
#include <sstream>

MapManager::MapManager()
    : coin_animation(16, "textures/coin.png", 8),
    question_block_animation(16, "textures/question_block.png", 16) {

    // Cargar texturas
    if (!map_texture.loadFromFile("textures/tileset.png")) {
        std::cout << "Error: No se pudo cargar tileset.png" << std::endl;
    }

    if (!spikes_texture.loadFromFile("textures/spikes.png")) {
        std::cout << "Error: No se pudo cargar spikes.png" << std::endl;
    }

    if (!door_texture.loadFromFile("textures/door.png")) {
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

unsigned short MapManager::get_map_width() const {
    return map.empty() ? 0 : map[0].size();
}

unsigned short MapManager::get_map_height() const {
    return map.size();
}

void MapManager::load_map_from_text(int level) {
    std::string filename = "maps/level_" + std::to_string(level) + ".txt";
    std::ifstream file(filename);

    if (!file.is_open()) {
        std::cout << "Error: No se pudo abrir el archivo " << filename << std::endl;
        return;
    }

    std::vector<std::string> lines;
    std::string line;

    // Leer todas las líneas del archivo
    while (std::getline(file, line)) {
        lines.push_back(line);
    }
    file.close();

    if (lines.empty()) {
        std::cout << "Error: El archivo " << filename << " está vacío" << std::endl;
        return;
    }

    // Configurar el tamaño del mapa
    int height = lines.size();
    int width = 0;

    // Encontrar el ancho máximo
    for (const auto& l : lines) {
        width = std::max(width, static_cast<int>(l.length()));
    }

    // Redimensionar el mapa
    map.clear();
    map.resize(height);
    for (int i = 0; i < height; i++) {
        map[i].resize(width, Cell::Empty);
    }

    // Llenar el mapa según los caracteres
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < static_cast<int>(lines[y].length()); x++) {
            char c = lines[y][x];
            Cell cell = Cell::Empty;

            switch (c) {
            case '#': cell = Cell::Wall; break;      // Pared/bloque sólido
            case 'G': cell = Cell::Grass; break;     // Césped
            case 'L': cell = Cell::Light; break;     // Luz/moneda
            case 'H': cell = Cell::Life; break;      // Vida/corazón
            case 'S': cell = Cell::Entrance; break;  // Spawn del jugador
            case 'P': cell = Cell::Prock; break;     // Roca/piedra
            case '^': cell = Cell::Spikes; break;    // Pinchos
            case 'D': cell = Cell::Door; break;      // Puerta
            case ' ':
            case '.':
            default:  cell = Cell::Empty; break;     // Espacio vacío
            }

            map[y][x] = cell;
        }
    }

    std::cout << "Mapa cargado: " << filename << " (" << width << "x" << height << ")" << std::endl;
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

    // Actualizar animaciones
    coin_animation.update();
    question_block_animation.update();

    // Dibujar partículas y efectos
    for (auto& particle : brick_particles) {
        cell_sprite.setPosition(particle.x - view_x, particle.y);
        window.draw(cell_sprite);
    }

    for (auto& coin : question_block_coins) {
        coin_animation.set_position(coin.x - view_x, coin.y);
        coin_animation.draw(window);
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

void MapManager::add_brick_particles(float x, float y) {
    for (int i = 0; i < 4; i++) {
        float hspeed = (i % 2 == 0) ? -2.0f : 2.0f;
        float vspeed = (i < 2) ? -3.0f : -1.0f;
        brick_particles.emplace_back(x, y, hspeed, vspeed);
    }
}

void MapManager::add_question_block_coin(float x, float y) {
    question_block_coins.emplace_back(x, y, 0.0f, -4.0f);
}

void MapManager::update() {
    // Actualizar partículas
    for (auto it = brick_particles.begin(); it != brick_particles.end();) {
        it->x += it->horizontal_speed;
        it->y += it->vertical_speed;
        it->vertical_speed += 0.2f; // Gravedad

        if (it->y > 240) { // Altura de pantalla
            it = brick_particles.erase(it);
        }
        else {
            ++it;
        }
    }

    // Actualizar monedas
    for (auto it = question_block_coins.begin(); it != question_block_coins.end();) {
        it->y += it->vertical_speed;
        it->vertical_speed += 0.2f;

        if (it->vertical_speed > 0 && it->y > it->y - 32) {
            it = question_block_coins.erase(it);
        }
        else {
            ++it;
        }
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