#pragma once
#include "Game.h"

enum class TileType {
    EMPTY = 0,
    SOLID_BLOCK = 1,
    SPIKE = 2,
    DOOR = 3,
    TORCH = 4,
    PLATFORM = 5,  // Traspasable desde abajo
    // Tileset blocks con diferentes orientaciones
    BLOCK_TOP_LEFT = 6,
    BLOCK_TOP = 7,
    BLOCK_TOP_RIGHT = 8,
    BLOCK_LEFT = 9,
    BLOCK_CENTER = 10,
    BLOCK_RIGHT = 11,
    BLOCK_BOTTOM_LEFT = 12,
    BLOCK_BOTTOM = 13,
    BLOCK_BOTTOM_RIGHT = 14,
    // Bloques con sombras
    BLOCK_SHADOW_TOP = 15,
    BLOCK_SHADOW_LEFT = 16,
    BLOCK_SHADOW_RIGHT = 17,
    BLOCK_SHADOW_BOTTOM = 18
};

class LevelEditor {
public:
    LevelEditor(Map& map, float cellSize = 32.0f);

    void HandleInput(const sf::Event& event, const sf::RenderWindow& window);
    void Update(float deltaTime);
    void Draw(Renderer& renderer);
    void DrawUI(sf::RenderWindow& window);

    void SetSelectedTile(TileType type);
    void SaveLevel(const std::string& filename);
    void LoadLevel(const std::string& filename);
    void ClearLevel();
    void NewLevel(int width, int height);

    bool IsEditorMode() const { return editorMode; }
    void SetEditorMode(bool mode) { editorMode = mode; }

    // Función para verificar si un tile es sólido para colisiones
    static bool IsSolidTile(TileType type);
    // Función para verificar si un tile es una plataforma (traspasable desde abajo)
    static bool IsPlatformTile(TileType type);
    // Función para verificar si un tile causa daño
    static bool IsDamageTile(TileType type);
    // Función para verificar si un tile es animado
    static bool IsAnimatedTile(TileType type);

private:
    Map* mapRef;
    float cellSize;
    bool editorMode;
    TileType selectedTileType;
    bool isPlacing;
    bool isErasing;
    sf::Vector2i lastMouseGrid;

    // UI elements
    sf::Font font;
    sf::Text instructionText;
    sf::RectangleShape tileSelector;
    sf::Text tileTypeText;
    sf::Text tileInfoText;

    // Grid helper
    sf::Vector2i ScreenToGrid(const sf::Vector2i& screenPos, const sf::RenderWindow& window);
    sf::Vector2f GridToScreen(const sf::Vector2i& gridPos);
    void PlaceTile(const sf::Vector2i& gridPos);
    void EraseTile(const sf::Vector2i& gridPos);
    void DrawGrid(Renderer& renderer, const sf::View& view);
    void InitUI();
    std::string TileTypeToString(TileType type);
    void UpdateTileSelection();
    void DrawTilePreview(Renderer& renderer);
};