#include "Global.h"
#include "MapManager.h"
#include <fstream>
#include <sstream>
#include "filesystem"

MapManager::MapManager() {
    // Cargar texturas
    if (!map_texture.loadFromFile("assets/World/bases.png")) { // Asumiendo que esta es la textura del tileset principal
        std::cout << "Error: No se pudo cargar assets/World/bases.png" << std::endl;
    }

    if (!spikes_texture.loadFromFile("assets/World/spikes.png")) {
        std::cout << "Error: No se pudo cargar assets/World/spikes.png" << std::endl;
    }

    if (!door_texture.loadFromFile("assets/World/door.png")) {
        std::cout << "Error: No se pudo cargar assets/World/door.png" << std::endl;
    }

    cell_sprite.setTexture(map_texture); // Configurar textura por defecto para cell_sprite
}

void MapManager::load_map_from_text(int level) {
    std::string filename = "maps/level" + std::to_string(level) + ".txt";
    std::ifstream file(filename);

    if (!file.is_open()) {
        std::cout << "Error: Could not open " << filename << std::endl;
        // Crear un mapa predeterminado
        game_map.assign(15, std::vector<Cell>(20, Cell::Empty)); // Usar game_map
        // Add some ground
        for (int i = 0; i < 20; i++) {
            game_map[14][i] = Cell::Wall; // Usar game_map
        }
        return;
    }

    // Clear existing map
    game_map.clear(); // Usar game_map

    std::string line;
    // int row = 0; // No se usa 'row' para limitar aquí, se lee todo el archivo

    while (std::getline(file, line)) {
        std::vector<Cell> map_row;
        unsigned int current_cols = 0; // Para rastrear columnas leídas para esta fila

        for (char c : line) {
 
            Cell cell = Cell::Empty;
            switch (c) {
            case '#': cell = Cell::Wall; break;
            case 'G': cell = Cell::Grass; break; // Asumiendo que G es Grass
                // case 'L': cell = Cell::Light; break; // No definido en el enum Cell de Global.h?
                                                     // Cell::Light = 3; sí está.
            case 'H': cell = Cell::Life; break;  // H se usa para Life en Global.cpp, pero no es un tipo de celda
                // para el mapa, sino un objeto. 
                // El enum Cell tiene Cell::Life = 4
            case 'E': cell = Cell::Entrance; break;
            case 'R': cell = Cell::Prock; break;
            case 'S': cell = Cell::Spikes; break;
            case 'D': cell = Cell::Door; break;
                // Caracteres que representan entidades, no celdas de mapa
            case 'P': // Player position
            case 'A': // Ant position
            case 'a': // Aggressive ant
            case 'M': // ReyHongo
            case 'm':
                cell = Cell::Empty; // Estas posiciones deberían ser vacías en el mapa de colisión
                break;
            case ' ': // Espacio explícitamente vacío
            case '.': // Otro carácter para vacío
            default:
                cell = Cell::Empty;
                break;
            }
            map_row.push_back(cell);
            current_cols++;
        }
        game_map.push_back(map_row); // Usar game_map
    }

    file.close();

    // Normalizar el mapa: asegurar que todas las filas tengan el mismo ancho (el de la fila más larga)
    unsigned int max_width = 0;
    for (const auto& row_vec : game_map) {
        if (row_vec.size() > max_width) {
            max_width = row_vec.size();
        }
    }
    for (auto& row_vec : game_map) {
        row_vec.resize(max_width, Cell::Empty);
    }


    std::cout << "Loaded map from " << filename << " with dimensions " << get_map_width() << "x" << get_map_height() << std::endl;
}

unsigned short MapManager::get_map_width() const {
    return game_map.empty() ? 0 : game_map[0].size(); // Usar game_map
}

unsigned short MapManager::get_map_height() const {
    return game_map.size(); // Usar game_map
}

void MapManager::draw_map(bool draw_background, int level, float view_x, sf::RenderWindow& window) {
    const int CELL_SIZE_CONST = 16; // Usar una constante local o global consistente
    // No usar la variable `level` aquí, ya que el mapa ya está cargado en `game_map`
    // `draw_background` tampoco se usa aquí.

    // Calcular el rango de columnas a dibujar
    // Asegurarse de que view_x no sea negativo si el mapa es más pequeño que la pantalla.
    float corrected_view_x = (view_x < 0) ? 0 : view_x;

    int start_col = static_cast<int>(corrected_view_x / CELL_SIZE_CONST);
    if (start_col < 0) start_col = 0; // No dibujar columnas negativas

    // +1 para asegurar que se dibuja la celda parcial del final, +1 extra por si acaso.
    int end_col = start_col + (static_cast<int>(window.getSize().x / WINDOW_SCALE) / CELL_SIZE_CONST) + 2;


    if (end_col > static_cast<int>(get_map_width())) {
        end_col = static_cast<int>(get_map_width());
    }

    for (int r = 0; r < static_cast<int>(get_map_height()); ++r) {
        for (int c = start_col; c < end_col; ++c) {

            Cell cell_type = game_map[r][c]; 
            float pos_x = c * CELL_SIZE_CONST - corrected_view_x;
            float pos_y = r * CELL_SIZE_CONST;

            if (cell_type != Cell::Empty) {
                sf::Sprite current_cell_sprite; // Crear un sprite temporal para cada celda


                if (cell_type == Cell::Spikes) {
                    cell_sprite.setTexture(spikes_texture);
                    cell_sprite.setTextureRect(sf::IntRect(0, 0, spikes_texture.getSize().x, spikes_texture.getSize().y)); // Usar todo el sprite de pinchos
                }
                else if (cell_type == Cell::Door) {
                    cell_sprite.setTexture(door_texture);
                    cell_sprite.setTextureRect(sf::IntRect(0, 0, door_texture.getSize().x, door_texture.getSize().y)); // Usar todo el sprite de puerta
                }
                else {
                    // Celda normal del tileset principal (map_texture)
                    cell_sprite.setTexture(map_texture);
                    int tex_x = static_cast<int>(cell_type) * CELL_SIZE_CONST; // Asume que el enum Cell corresponde a tiles horizontales
                    // Necesitas saber cómo está organizado tu tileset (map_texture)
                    // Si cada tipo de celda es un frame de 16x16 en una fila:
                    cell_sprite.setTextureRect(sf::IntRect(tex_x, 0, CELL_SIZE_CONST, CELL_SIZE_CONST));
                    // Si es más complejo (como en DrawMap.cpp), esa lógica debería estar aquí o ser llamada.
                    // La implementación actual en DrawMap.cpp original es más avanzada para 'Ground'.
                    // Si se quiere ese "autotiling", se necesita replicar aquí o fusionar DrawMap.cpp con MapManager.
                }
                cell_sprite.setPosition(pos_x, pos_y);
                window.draw(cell_sprite);
            }
        }
    }
}


void MapManager::set_map_cell(int x, int y, Cell cell) {
    if (y >= 0 && y < static_cast<int>(game_map.size()) &&
        x >= 0 && x < static_cast<int>(game_map[y].size())) {
        game_map[y][x] = cell; // Usar game_map
    }
}

// Implementación para set_map_size(width, height)
void MapManager::set_map_size(unsigned int new_width, unsigned int new_height) {
    game_map.resize(new_height); // Usar game_map
    for (auto& row : game_map) { // Usar game_map
        row.resize(new_width, Cell::Empty);
    }
}

std::vector<unsigned char> MapManager::map_collision(const std::vector<Cell>& check_cells,
    const sf::FloatRect& hitbox) {
    std::vector<unsigned char> collisions_indices; // Renombrado para claridad
    const int CELL_SIZE_CONST = 16;

    // Calcular las celdas del mapa que el hitbox podría estar tocando
    int left_col = static_cast<int>(hitbox.left / CELL_SIZE_CONST);
    int right_col = static_cast<int>((hitbox.left + hitbox.width - 1) / CELL_SIZE_CONST); // -1 para no pasarse a la siguiente celda si está justo en el borde
    int top_row = static_cast<int>(hitbox.top / CELL_SIZE_CONST);
    int bottom_row = static_cast<int>((hitbox.top + hitbox.height - 1) / CELL_SIZE_CONST); // -1 similar

    for (int r = top_row; r <= bottom_row; ++r) {
        for (int c = left_col; c <= right_col; ++c) {
            // Comprobar límites del mapa
            if (r >= 0 && r < static_cast<int>(game_map.size()) &&
                c >= 0 && c < static_cast<int>(game_map[r].size())) { // Usar game_map

                Cell cell_in_map = game_map[r][c]; // Usar game_map
                for (size_t i = 0; i < check_cells.size(); ++i) {
                    if (cell_in_map == check_cells[i]) {
                        collisions_indices.push_back(static_cast<unsigned char>(i));
                        // No hacer break aquí si se quieren todas las colisiones dentro del hitbox,
                        // pero si solo se necesita saber SI hay colisión con *alguna* de check_cells,
                        // podrías retornar true o el índice inmediatamente.
                        // La implementación original recolecta todos los índices.
                    }
                }
            }
        }
    }
    return collisions_indices;
}

// Esta sobrecarga ignora collision_cells_ignored
std::vector<unsigned char> MapManager::map_collision(const std::vector<Cell>& check_cells,
    const std::vector<Cell>& collision_cells_ignored,
    const sf::FloatRect& hitbox) {
    // Advertir que el segundo parámetro no se usa si se mantiene esta firma
    // std::cout << "Advertencia: MapManager::map_collision (3 params) ignora el segundo vector de celdas." << std::endl;
    return map_collision(check_cells, hitbox); // Llama a la otra versión
}

