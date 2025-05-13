#pragma once

enum TileType {
    EMPTY = 0,
    SOLID,      // Bloque sólido (paredes)
    PLATFORM,   // Plataforma (se puede saltar desde abajo)
    LADDER,     // Escalera (permite subir y bajar)
    SPIKE,      // Pinchos (daña al jugador)
    DECORATION  // Elementos decorativos (sin colisión)
};

class Tile
{
private:
    sf::Sprite sprite;
    TileType type;
    bool damaging;

public:
    Tile(const unsigned& grid_pos_x, const unsigned& grid_pos_y, const unsigned& tile_size,
        sf::Texture* texture_sheet, sf::IntRect texture_rect, TileType type, bool damaging = false);

    // Getters
    const sf::FloatRect getGlobalBounds() const;
    const TileType& getType() const;
    const sf::Vector2f& getPosition() const;
    const bool isDamaging() const;

    void update();
    void render(sf::RenderTarget& target);
};