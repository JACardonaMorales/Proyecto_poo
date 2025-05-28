#include "Global.h"
#include "ReyHongo.h"
#include "MapManager.h"
#include "Ray.h"
#include <iostream> 
#include <cmath>    

ReyHongo::ReyHongo(bool attacks, float x, float y)
    : Enemy(x, y),
    // Corregir rutas de texturas para consistencia (asumiendo que están en 'assets/ReyHongo/')
    get_out_animation(1600, "assets/King/ahora_si_sapa.png", 9),
    walk_animation(500, "assets/King/stand_animation-Sheet.png", 12),
    attacks(attacks), check_collision(true), flipped(false),
    no_collision_dying(false), state(0), get_out_timer(0) {

    if (!texture.loadFromFile("assets/King/stand_animation-Sheet.png")) { // Ruta corregida/asumida
        std::cerr << "Error: No se pudo cargar la textura rey_hongo.png" << std::endl;
    }
    sprite.setTexture(texture);

    horizontal_speed = attacks ? 1.2f : 0.8f;
}

bool ReyHongo::get_dead(bool deletion) {
    if (deletion && state == 3 && dead) { // Asegurarse que 'dead' también sea true
        return true;
    }
    return dead; // Devuelve el estado 'dead' general. La clase derivada decide cuándo se elimina realmente.
}

void ReyHongo::die(int death_type) {
    if (!dead && state != 3) {
        dead = true; // Marcar como muerto lógicamente
        state = 3;   // Estado visual/lógico de muerto
        if (death_type == 0) { // Stomp kill
            vertical_speed = -4.0f;
            no_collision_dying = true; // Para la animación de muerte con física
        }
        // else if (death_type == 1) { // Fire kill - ya está en estado 3 y dead = true
        // }
    }
}

void ReyHongo::draw(float view_x, sf::RenderWindow& window) {
    if (state == 3 && dead) { // Solo dibujar si está realmente en el estado de "muerto"
        // Draw dead sprite upside down
        sprite.setPosition(x - view_x, y);
        // Considerar el origen para la escala si no es el centro
        // sprite.setOrigin(sprite.getLocalBounds().width / 2.f, sprite.getLocalBounds().height / 2.f);
        sprite.setScale(flipped ? -1.0f : 1.0f, -1.0f); // Escala invertida para "muerto"
        window.draw(sprite);
        return;
    }

    if (dead) return; // Si está lógicamente muerto pero no en el estado 3 final, no dibujar (o manejar diferente)

    switch (state) {
    case 0: // Emerging
        get_out_animation.set_position(x - view_x, y);
        get_out_animation.set_flipped(flipped); // Flipped podría no tener sentido aquí
        get_out_animation.draw(window);
        break;

    case 1: // Walking
        if (horizontal_speed != 0) {
            walk_animation.set_position(x - view_x, y);
            walk_animation.set_flipped(flipped);
            walk_animation.draw(window);
        }
        else {
            sprite.setPosition(x - view_x, y);
            sprite.setScale(flipped ? -1.0f : 1.0f, 1.0f);
            window.draw(sprite);
        }
        break;

    case 2: // Hidden
        // Don't draw when hidden
        break;
    }
}

void ReyHongo::update(float view_x, std::vector<std::shared_ptr<Enemy>>& enemies,
    MapManager& map_manager, Ray& ray) {

    if (state == 3 && dead) { // Si está en el estado de muerto final
        if (no_collision_dying) {
            vertical_speed += 0.2f; // GRAVITY_CONSTANT o similar
            y += vertical_speed;
            // Podrías añadir una condición para que se elimine después de un tiempo o al salir de pantalla
        }
        return; // No hacer más actualizaciones
    }

    // Si está lógicamente 'dead' pero no en 'state 3' (p.ej. esperando animación de muerte)
    // podrías tener lógica aquí, pero por ahora si 'dead' es true y no es state 3, no actualiza mucho.

    switch (state) {
    case 0: // Emerging
    {
        get_out_timer++;
        get_out_animation.update();
        if (get_out_timer >= 60) { // 1 second (asumir 60 FPS)
            state = 1; // Cambiar a estado Walking
            get_out_timer = 0;
        }
        break;
    }
    case 1: // Walking
    {
        // Movement
        x += horizontal_speed;
        flipped = horizontal_speed < 0;

        // Apply gravity
        vertical_speed += 0.2f; // Usar una constante de gravedad
        y += vertical_speed;

        // Check collisions with map
        if (check_collision) {
            std::vector<Cell> solid_cells = { Cell::Wall, Cell::Grass, Cell::Prock }; // Debería ser de Global.h
            sf::FloatRect current_hit_box = get_hit_box();
            auto collisions = map_manager.map_collision(solid_cells, current_hit_box);

            if (!collisions.empty()) {
                // Un manejo de colisión más robusto sería necesario aquí.
                // Esto es una simplificación.
                if (vertical_speed > 0) { // Colisión inferior (suelo)
                    y = static_cast<int>(y / 16) * 16; // Ajustar a la rejilla (CELL_SIZE)
                    vertical_speed = 0.0f;
                }
                else if (vertical_speed < 0) { // Colisión superior (techo)
                    y = (static_cast<int>(y / 16) + 1) * 16;
                    vertical_speed = 0.0f;
                }
                // Podrías necesitar ajustar X también si choca lateralmente
            }

            // Check for edges or walls to turn around
            // Comprobación de suelo DELANTE para evitar caer
            sf::FloatRect front_check(x + (horizontal_speed > 0 ? 16 : -16), y + 16, 1, 1); // Comprobar un punto
            auto ground_ahead_collisions = map_manager.map_collision(solid_cells, front_check);
            bool is_ground_solid_ahead = false; // Necesitas una forma de saber si hay suelo para caminar, no si hay una pared en el pie

            // Esta lógica necesita ser más precisa. El front_check actual es para detectar si NO hay suelo
            // Si front_collisions.empty() significa que no hay celda sólida (como Cell::Wall) *debajo* del siguiente paso
            // pero esto no es lo mismo que "no hay suelo". Necesitarías comprobar map[next_x][y+1]
            // Para simplificar, la lógica original de front_collisions.empty() para girar si no hay suelo debajo se mantiene
            // pero requiere que el tileset sea consistente.

            // Comprobación de pared en frente
            sf::FloatRect wall_check_rect(x + (horizontal_speed > 0 ? current_hit_box.width : -1.0f), y, 1.0f, current_hit_box.height - 1.0f);
            auto wall_collisions = map_manager.map_collision(solid_cells, wall_check_rect);

            if (!wall_collisions.empty()) {
                horizontal_speed *= -1;
                x += horizontal_speed > 0 ? 1.0f : -1.0f; // Pequeño empujón para evitar atascarse
            }
            else {
                // Comprobar si hay un precipicio
                sf::FloatRect cliff_check_pos;
                if (horizontal_speed > 0) {
                    cliff_check_pos = sf::FloatRect(x + current_hit_box.width, y + current_hit_box.height, 1, 1);
                }
                else {
                    cliff_check_pos = sf::FloatRect(x - 1, y + current_hit_box.height, 1, 1);
                }
                auto cliff_check_collisions = map_manager.map_collision(solid_cells, cliff_check_pos);
                if (cliff_check_collisions.empty()) { // No hay suelo delante
                    horizontal_speed *= -1;
                }
            }
        }

        // Check collision with Ray
        if (get_hit_box().intersects(ray.get_hit_box())) {
            if (!ray.get_dead() && ray.get_vertical_speed() > 0 && ray.get_y() < y - get_hit_box().height / 2.0f) { // Ray saltando encima
                die(0); // Stomp kill
                ray.set_vertical_speed(-3.0f); // Bounce ray up
            }
            else if (!ray.get_dead()) { // Cualquier otra colisión daña a Ray
                ray.die(false);
            }
        }

        // Update walking animation
        if (horizontal_speed != 0) {
            walk_animation.update();
        }

        // Hide if Ray is too far
        float ray_distance = std::abs(ray.get_x() - x); // Usar std::abs y la variable correcta
        if (ray_distance > 100) { // DISTANCE_TO_HIDE o similar
            state = 2; // Cambiar a estado Hidden
            get_out_timer = 0; // Reiniciar temporizador para re-emerger
        }
        break;
    }
    case 2: // Hidden
    {
        get_out_timer++;
        // Re-emerge si Ray se acerca o después de un tiempo
        float ray_distance = std::abs(ray.get_x() - x);
        if (ray_distance < 80 || get_out_timer > 300) { // DISTANCE_TO_EMERGE, MAX_HIDE_TIME (5 segundos)
            state = 0; // Volver a emerger
            get_out_timer = 0;
            // Podrías querer que mire hacia Ray al re-emerger
            if (ray.get_x() < x) horizontal_speed = -std::abs(horizontal_speed);
            else horizontal_speed = std::abs(horizontal_speed);
            flipped = horizontal_speed < 0;

        }
        break;
    }
    } // Cierre del switch(state)

    // Remove if too far from view (esto se aplica a todos los estados excepto muerto final)
    if (x < view_x - 100 || x > view_x + SCREEN_WIDTH + 100) { 
    	std::cout << "ReyHongo too far, marking as dead." << std::endl;
       
    }
} 

