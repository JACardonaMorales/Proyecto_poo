#include "Global.h"
#include "MapManager.h" // MapManager necesita conocer Cell y Map de Global.h
#include "Ray.h"
#include "Enemy.h"
#include "Infected_Ant.h"
#include "ReyHongo.h"
#include <memory>
#include <fstream>
#include <string>   // Para std::to_string
#include <iostream> // Para std::cout, std::cerr

// Función para cargar el mapa y entidades desde un archivo de texto
// level_finish no se usa actualmente en esta implementación.
sf::Color convert_sketch_from_text(int level, bool& /*level_finish_unused*/,
    std::vector<std::shared_ptr<Enemy>>& enemies,
    sf::Color bg_color, MapManager& map_manager, Ray& ray) {

    // Cargar la estructura del mapa desde archivo de texto usando MapManager
    map_manager.load_map_from_text(level); // MapManager ya se encarga de cargar las celdas del mapa

    // Limpiar enemigos existentes antes de cargar nuevos
    enemies.clear();

    // Ahora, leer el mismo archivo de texto para buscar entidades (Ray, Enemigos)
    // Esto es redundante si main.cpp::loadLevelData ya lo hace.
    // Se debería unificar quién es responsable de cargar las entidades.
    // Si Global.cpp lo hace, entonces main.cpp no debería.
    // Si main.cpp lo hace, esta parte de Global.cpp es redundante.
    // Por ahora, mantendré la lógica como estaba, pero marcando la redundancia.
    std::cout << "ADVERTENCIA: convert_sketch_from_text en Global.cpp está cargando entidades, lo cual puede ser redundante con main.cpp::loadLevelData." << std::endl;

    std::string filename = "maps/level_" + std::to_string(level) + ".txt";
    std::ifstream file(filename);

    if (!file.is_open()) {
        std::cerr << "Error en Global.cpp: No se pudo abrir " << filename << " para buscar entidades" << std::endl;
        // Establecer una posición por defecto para Ray si el archivo no se puede abrir
        ray.set_position(CELL_SIZE * 2, CELL_SIZE * 2); // Posición segura
        return bg_color; // O un color de error
    }

    std::string line;
    int y_coord = 0; // Coordenada de fila en el mapa

    while (std::getline(file, line)) {
        for (int x_coord = 0; x_coord < static_cast<int>(line.length()); ++x_coord) {
            char entity_char = line[x_coord];
            float world_x = x_coord * CELL_SIZE; // Usar CELL_SIZE global
            float world_y = y_coord * CELL_SIZE; // Usar CELL_SIZE global

            switch (entity_char) {
            case 'P': // Spawn del jugador (reemplazando 'S' si 'P' es el estándar)
                // O mantener 'S' si se usa en los archivos de nivel.
                // El código original usa 'S' para Entrance y también para Ray spawn
                // Es mejor tener un carácter único para el spawn del jugador.
                // Asumiendo que 'P' es para el jugador (como en main.cpp)
                ray.set_position(world_x, world_y);
                // Asegurarse de que la celda del mapa sea 'Empty' donde aparece el jugador
                map_manager.set_map_cell(x_coord, y_coord, Cell::Empty);
                break;
                // Los casos 'S', 'A', 'a', 'M', 'm' del código original de Global.cpp
                // estaban aquí. Si main.cpp::loadLevelData es la fuente autorizada para enemigos,
                // esta sección se vuelve redundante.
                // El original tenía:
                // case 'S': ray.set_position(world_x, world_y); break;
                // case 'A': enemies.push_back(std::make_shared<Infected_Ant>(false, world_x, world_y)); break;
                // case 'a': enemies.push_back(std::make_shared<Infected_Ant>(true, world_x, world_y)); break;
                // case 'M': enemies.push_back(std::make_shared<ReyHongo>(false, world_x, world_y)); break;
                // case 'm': enemies.push_back(std::make_shared<ReyHongo>(true, world_x, world_y)); break;
                // Estos deberían coincidir con los de main.cpp::loadLevelData si se unifica.
            }
        }
        y_coord++;
    }
    file.close();

    std::cout << "Entidades (potencialmente) cargadas desde Global.cpp: " << enemies.size() << " enemigos." << std::endl;
    return bg_color;
}

// Función original para compatibilidad con imágenes
// level_finish no se usa.

// Esta función draw_map es de Global.cpp y usa un sf::Image (map_sketch)
// y un std::vector<std::vector<Cell>> (map_data) que se le pasa.
// Es diferente de MapManager::draw_map.
void draw_map(float view_x, const sf::Image& /*map_sketch_unused*/, sf::RenderWindow& window,
    const sf::Texture& map_tileset_texture, const std::vector<std::vector<Cell>>& map_data) { // map_sketch no se usa aquí

    sf::Sprite cell_sprite;
    cell_sprite.setTexture(map_tileset_texture); // Usar la textura de tileset pasada

    // CELL_SIZE debería ser la constante global
    int start_x_col = static_cast<int>(view_x / CELL_SIZE);
    // int end_x_col = start_x_col + (window.getSize().x / CELL_SIZE) + 2; // Asumiendo que window.getSize().x es el ancho de la vista del juego
                                                                     // Si se usa la cámara escalada de main.cpp:
                                                                     // (WINDOW_WIDTH / CELL_SIZE) + 2
    int end_x_col = start_x_col + (SCREEN_WIDTH / CELL_SIZE) + 2;


    for (int r = 0; r < static_cast<int>(map_data.size()); ++r) {
        if (r < 0) continue; // Seguridad, aunque 'r' debería ser >= 0
        for (int c = start_x_col; c < end_x_col; ++c) {
            if (c < 0 || c >= static_cast<int>(map_data[r].size())) continue; // Comprobar límites de columna

            Cell cell_type = map_data[r][c];
            if (cell_type != Cell::Empty) {
                // Aquí se asume que el enum Cell mapea directamente a los tiles en map_tileset_texture
                // donde cada tile es CELL_SIZE x CELL_SIZE en una fila horizontal.
                int tex_x = static_cast<int>(cell_type) * CELL_SIZE;
                cell_sprite.setTextureRect(sf::IntRect(tex_x, 0, CELL_SIZE, CELL_SIZE));

                // La posición de dibujado es relativa a la ventana, no a la vista global del mapa
                // view_x ya está aplicado para las columnas.
                cell_sprite.setPosition(c * CELL_SIZE - view_x, r * CELL_SIZE);
                window.draw(cell_sprite);
            }
        }
    }
}

// Función de colisión de Global.cpp
unsigned char map_collision(float world_pos_x, float world_pos_y, const std::vector<Cell>& cells_to_check,
    const std::vector<std::vector<Cell>>& map_data) {

    // CELL_SIZE debería ser la constante global
    int map_col = static_cast<int>(world_pos_x / CELL_SIZE);
    int map_row = static_cast<int>(world_pos_y / CELL_SIZE);

    // Comprobar límites del mapa
    if (map_row >= 0 && map_row < static_cast<int>(map_data.size()) &&
        map_col >= 0 && map_col < static_cast<int>(map_data[map_row].size())) {

        Cell cell_in_map = map_data[map_row][map_col];

        for (size_t i = 0; i < cells_to_check.size(); ++i) {
            if (cell_in_map == cells_to_check[i]) {
                return static_cast<unsigned char>(i); // Devuelve el ÍNDICE en cells_to_check
            }
        }
    }
    return 255; // No collision (o un valor que indique no colisión)
}