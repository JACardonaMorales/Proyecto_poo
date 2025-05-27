#pragma once
#include "Types.h"

class Map;
class Renderer;
class TileRenderer;

class LevelEditor
{
public:
    LevelEditor();
    ~LevelEditor();

    void Initialize();
    void CreatePlaceholderTextures();
    std::string GetTextureNameForTileType(TileType type) const;
    void HandleInput(const sf::Event& event, const sf::RenderWindow& window);
    void Update(float deltaTime);
    void Render(sf::RenderWindow& window);
    void RenderGrid(sf::RenderWindow& window);
    void RenderUI(sf::RenderWindow& window);
    bool isActive;

    // Tile management
    void SetCurrentTile(TileType type);
    void PlaceTile(int x, int y);
    void RemoveTile(int x, int y);
    TileType GetTileAt(int x, int y) const;
    bool HasCollisionAt(int x, int y) const;

    // Método mejorado para validar posiciones de grid:
    bool IsValidGridPosition(int x, int y) const;


    // File operations
    void SaveLevel(const std::string& filename);
    void LoadLevel(const std::string& filename);
    void ExportAsImage(const std::string& filename);
    void NewLevel(int width, int height);

    // Getters
    const std::vector<std::vector<TileType>>& GetGrid() const { return grid; }
    int GetGridWidth() const { return gridWidth; }
    int GetGridHeight() const { return gridHeight; }
    float GetTileSize() const { return tileSize; }

    // Métodos de acceso necesarios para Game.cpp
    void SetActive(bool active) { isActive = active; }
    bool IsActive() const { return isActive; }


    // Método para renderizar usando RenderWindow directamente
    void Render(sf::RenderTarget& target) {
        sf::RenderWindow* window = dynamic_cast<sf::RenderWindow*>(&target);
        if (window) {
            Render(*window);
        }
    }

private:
    // Grid data
    std::shared_ptr<TileRenderer> tileRenderer;
    std::vector<std::vector<TileType>> grid;
    int gridWidth;
    int gridHeight;
    float tileSize;

    // Editor state
    TileType currentTileType;
   
    bool showGrid;
    bool isPainting;
    bool isErasing;

    // Camera and view
    sf::View editorView;
    sf::Vector2f cameraPosition;
    float zoomLevel;

    // UI elements
    sf::Font font;
    std::vector<sf::Text> tileButtons;
    sf::RectangleShape selectedTileIndicator;
    sf::Text infoText;

    // Tile information
    std::vector<TileInfo> tileInfos;

    // Animation
    sf::Clock animationClock;
    float torchAnimationTime;

    // Helper functions
    void InitializeTileInfos();
    void LoadTextures();
    sf::Vector2i GetGridPosition(const sf::Vector2f& worldPos) const;
    sf::Vector2f GetWorldPosition(int gridX, int gridY) const;
    void UpdateCamera();
    void RenderTile(sf::RenderWindow& window, TileType type, const sf::Vector2f& position);
    void RenderTorch(sf::RenderWindow& window, const sf::Vector2f& position, float animTime);
    void SetupUI();
    int GetTilesetIndex(TileType type) const;
    sf::IntRect GetTilesetRect(TileType type) const;
};