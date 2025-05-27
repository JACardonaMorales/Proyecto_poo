#include "Global.h"
#include "MapManager.h"

MapManager::MapManager()
    : Torch_Animation(16, "textures/coin.png", 8),
    question_block_animation(16, "textures/question_block.png", 16) {

    if (!map_texture.loadFromFile("textures/tileset.png")) {
        // Handle error
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

void MapManager::add_brick_particles(float x, float y) {
    // Add 4 particles for brick destruction
    for (int i = 0; i < 4; i++) {
        float hspeed = (i % 2 == 0) ? -2.0f : 2.0f;
        float vspeed = (i < 2) ? -3.0f : -1.0f;
        brick_particles.emplace_back(x, y, hspeed, vspeed);
    }
}

void MapManager::add_question_block_coin(float x, float y) {
    question_block_coins.emplace_back(x, y, 0.0f, -4.0f);
}

void MapManager::draw_map(bool draw_background, int level, float view_x, sf::RenderWindow& window) {
    const int CELL_SIZE = 16;
    int start_x = static_cast<int>(view_x / CELL_SIZE);
    int end_x = start_x + (window.getSize().x / CELL_SIZE) + 2;

    for (int y = 0; y < static_cast<int>(map.size()); y++) {
        for (int x = start_x; x < end_x && x < static_cast<int>(map[y].size()); x++) {
            if (x < 0) continue;

            Cell cell = map[y][x];
            if (cell != Cell::Empty) {
                // Set appropriate texture rectangle based on cell type
                int tex_x = static_cast<int>(cell) * CELL_SIZE;
                cell_sprite.setTextureRect(sf::IntRect(tex_x, 0, CELL_SIZE, CELL_SIZE));
                cell_sprite.setPosition(x * CELL_SIZE - view_x, y * CELL_SIZE);
                window.draw(cell_sprite);
            }
        }
    }

    // Draw animations
    coin_animation.update();
    question_block_animation.update();

    // Draw particles and coins
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

void MapManager::update() {
    // Update particles
    for (auto it = brick_particles.begin(); it != brick_particles.end();) {
        it->x += it->horizontal_speed;
        it->y += it->vertical_speed;
        it->vertical_speed += 0.2f; // Gravity

        if (it->y > 240) { // Screen height
            it = brick_particles.erase(it);
        }
        else {
            ++it;
        }
    }

    // Update coins
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
        // Handle error
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