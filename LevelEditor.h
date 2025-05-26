#pragma once
#include <SFML/Graphics.hpp>
#include <vector>

enum class TileType {
    EMPTY = 0,
    WALL = 1,
    PLATFORM = 2,
    SPIKES = 3,
    TORCH = 4,
    DOOR = 5,
    // Tileset blocks with different orientations
    BLOCK_TOP_LEFT = 10,
    BLOCK_TOP = 11,
    BLOCK_TOP_RIGHT = 12,
    BLOCK_LEFT = 13,
    BLOCK_CENTER = 14,
    BLOCK_RIGHT = 15,
    BLOCK_BOTTOM_LEFT = 16,
    BLOCK_BOTTOM = 17,
    BLOCK_BOTTOM_RIGHT = 18,
    // Shadow variations
    BLOCK_SHADOW_LEFT = 20,
    BLOCK_SHADOW_RIGHT = 21,
    BLOCK_SHADOW_BOTTOM = 22,
    BLOCK_SHADOW_CORNER = 23
};

struct TileInfo {
    TileType type;
    bool hasCollision;
    bool isAnimated;
    sf::Vector2i textureCoord; // Coordinates in tileset
    std::string textureName;

    TileInfo(TileType t, bool collision, bool animated, sf::Vector2i coord, const std::string& texture)
        : type(t), hasCollision(collision), isAnimated(animated), textureCoord(coord), textureName(texture) {
    }
};

class LevelEditor
{
public:
    LevelEditor();
    ~LevelEditor();

    void Initialize();
    void HandleInput(const sf::Event& event, const sf::RenderWindow& window);
    void Update(float deltaTime);
    void Render(sf::RenderWindow& window);
    void RenderGrid(sf::RenderWindow& window);
    void RenderUI(sf::RenderWindow& window);

    // Tile management
    void SetCurrentTile(TileType type);
    void PlaceTile(int x, int y);
    void RemoveTile(int x, int y);
    TileType GetTileAt(int x, int y) const;
    bool HasCollisionAt(int x, int y) const;

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

private:
    // Grid data
    std::vector<std::vector<TileType>> grid;
    int gridWidth;
    int gridHeight;
    float tileSize;

    // Editor state
    TileType currentTileType;
    bool isActive;
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