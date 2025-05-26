#include "stdafx.h"
#include "Physics.h"

CollisionInfo Physics::CheckCollision(Player& player, const Map& map) {
    CollisionInfo info = {};
    info.hasCollision = false;
    info.isGrounded = false;
    info.isOnPlatform = false;
    info.isOnLadder = false;
    info.takeDamage = false;
    info.correctionVector = sf::Vector2f(0, 0);
    info.tileType = TileType::EMPTY;

    sf::FloatRect playerBounds = player.getGlobalBounds();
    sf::Vector2f velocity = player.getVelocity();

    // Obtener todos los tiles que intersectan con el jugador
    std::vector<sf::Vector2i> tilesToCheck = GetTilesInBounds(playerBounds, map);

    for (const auto& tilePos : tilesToCheck) {
        CollisionInfo tileCollision = CheckTileCollision(playerBounds, map, tilePos.x, tilePos.y);

        if (tileCollision.hasCollision) {
            info.hasCollision = true;
            info.correctionVector += tileCollision.correctionVector;

            // Mantener información del tile más relevante
            if (tileCollision.takeDamage) {
                info.takeDamage = true;
                info.tileType = tileCollision.tileType;
            }

            if (tileCollision.isGrounded) {
                info.isGrounded = true;
            }

            if (tileCollision.isOnPlatform) {
                info.isOnPlatform = true;
            }

            if (tileCollision.isOnLadder) {
                info.isOnLadder = true;
            }
        }
    }

    return info;
}

CollisionInfo Physics::CheckTileCollision(const sf::FloatRect& bounds, const Map& map, int tileX, int tileY) {
    CollisionInfo info = {};
    info.hasCollision = false;
    info.isGrounded = false;
    info.isOnPlatform = false;
    info.isOnLadder = false;
    info.takeDamage = false;
    info.correctionVector = sf::Vector2f(0, 0);

    // Verificar que el tile esté dentro de los límites del mapa
    if (tileX < 0 || tileX >= static_cast<int>(map.grid[0].size()) ||
        tileY < 0 || tileY >= static_cast<int>(map.grid.size())) {
        return info;
    }

    TileType tileType = static_cast<TileType>(map.grid[tileY][tileX]);
    info.tileType = tileType;

    if (tileType == TileType::EMPTY) {
        return info;
    }

    sf::FloatRect tileBounds = GetTileBounds(tileX, tileY, map.cellSize);

    // Verificar intersección
    if (!bounds.intersects(tileBounds)) {
        return info;
    }

    // Manejar diferentes tipos de tiles
    switch (tileType) {
    case TileType::SPIKE:
        // Los pinchos causan daño pero no bloquean el movimiento
        info.takeDamage = true;
        info.hasCollision = false;
        break;

    case TileType::TORCH:
        // Las antorchas son decorativas, no tienen colisión
        info.hasCollision = false;
        break;

    case TileType::PLATFORM:
        // Las plataformas solo tienen colisión desde arriba
        if (CheckPlatformCollision(bounds, tileBounds, sf::Vector2f(0, 1))) {
            info.hasCollision = true;
            info.isOnPlatform = true;
            info.isGrounded = true;
            // Corrección solo hacia arriba
            float overlap = (bounds.top + bounds.height) - tileBounds.top;
            info.correctionVector = sf::Vector2f(0, -overlap);
        }
        break;

    case TileType::DOOR:
        // Las puertas pueden ser sólidas o no dependiendo del estado
        // Por ahora, tratarlas como sólidas
        info.hasCollision = true;
        info.correctionVector = CalculateCollisionCorrection(bounds, tileBounds);
        break;

    default:
        // Todos los demás tiles (bloques sólidos) tienen colisión completa
        if (LevelEditor::IsSolidTile(tileType)) {
            info.hasCollision = true;
            info.correctionVector = CalculateCollisionCorrection(bounds, tileBounds);

            // Verificar si el jugador está en el suelo
            float playerBottom = bounds.top + bounds.height;
            float tileTop = tileBounds.top;
            if (std::abs(playerBottom - tileTop) < 5.0f) {
                info.isGrounded = true;
            }
        }
        break;
    }

    return info;
}

bool Physics::CheckPlatformCollision(const sf::FloatRect& playerBounds, const sf::FloatRect& tileBounds, const sf::Vector2f& velocity) {
    // Solo hay colisión con plataformas si el jugador viene desde arriba
    float playerBottom = playerBounds.top + playerBounds.height;
    float tileTop = tileBounds.top;

    // El jugador debe estar cayendo (velocidad Y positiva) o ya estar cerca del tile
    bool comingFromAbove = velocity.y >= 0;
    bool nearTileTop = std::abs(playerBottom - tileTop) <= PLATFORM_TOLERANCE;

    return comingFromAbove && nearTileTop &&
        playerBounds.left < (tileBounds.left + tileBounds.width) &&
        (playerBounds.left + playerBounds.width) > tileBounds.left;
}

sf::Vector2f Physics::CalculateCollisionCorrection(const sf::FloatRect& bounds, const sf::FloatRect& tileBounds) {
    // Calcular el vector de corrección basado en la menor superposición
    sf::FloatRect intersection;
    bounds.intersects(tileBounds, intersection);

    sf::Vector2f correction(0, 0);

    // Determinar la dirección de menor penetración
    float overlapX = intersection.width;
    float overlapY = intersection.height;

    if (overlapX < overlapY) {
        // Corrección horizontal
        if (bounds.left < tileBounds.left) {
            correction.x = -overlapX; // Empujar a la izquierda
        }
        else {
            correction.x = overlapX;  // Empujar a la derecha
        }
    }
    else {
        // Corrección vertical
        if (bounds.top < tileBounds.top) {
            correction.y = -overlapY; // Empujar hacia arriba
        }
        else {
            correction.y = overlapY;  // Empujar hacia abajo
        }
    }

    return correction;
}

bool Physics::IsPointInTile(const sf::Vector2f& point, const Map& map) {
    int tileX = static_cast<int>(point.x / map.cellSize);
    int tileY = static_cast<int>(point.y / map.cellSize);

    if (tileX >= 0 && tileX < static_cast<int>(map.grid[0].size()) &&
        tileY >= 0 && tileY < static_cast<int>(map.grid.size())) {
        TileType tileType = static_cast<TileType>(map.grid[tileY][tileX]);
        return LevelEditor::IsSolidTile(tileType);
    }

    return false;
}

std::vector<sf::Vector2i> Physics::GetTilesInBounds(const sf::FloatRect& bounds, const Map& map) {
    std::vector<sf::Vector2i> tiles;

    // Calcular el rango de tiles que intersectan con el bounds
    int startX = static_cast<int>(bounds.left / map.cellSize);
    int endX = static_cast<int>((bounds.left + bounds.width) / map.cellSize);
    int startY = static_cast<int>(bounds.top / map.cellSize);
    int endY = static_cast<int>((bounds.top + bounds.height) / map.cellSize);

    // Asegurar que están dentro de los límites del mapa
    startX = std::max(0, startX);
    endX = std::min(static_cast<int>(map.grid[0].size()) - 1, endX);
    startY = std::max(0, startY);
    endY = std::min(static_cast<int>(map.grid.size()) - 1, endY);

    for (int y = startY; y <= endY; ++y) {
        for (int x = startX; x <= endX; ++x) {
            tiles.emplace_back(x, y);
        }
    }

    return tiles;
}