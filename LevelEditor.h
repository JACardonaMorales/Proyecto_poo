#pragma once
#include "Map.h"
#include "Renderer.h"

enum class TileType {
    EMPTY = 0,
    SOLID_BLOCK = 1,
    SPIKE = 2,
    DOOR = 3
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

private:
    Map& mapRef;
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

    // Grid helper
    sf::Vector2i ScreenToGrid(const sf::Vector2i& screenPos, const sf::View& view);
    sf::Vector2f GridToScreen(const sf::Vector2i& gridPos);
    void PlaceTile(const sf::Vector2i& gridPos);
    void EraseTile(const sf::Vector2i& gridPos);
    void DrawGrid(Renderer& renderer, const sf::View& view);
    void InitUI();
    std::string TileTypeToString(TileType type);
};
