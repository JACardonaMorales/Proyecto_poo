#include "stdafx.h"
#include "LevelEditor.h"
#include "Resources.h"
#include "Camera.h"

extern Camera camera;

LevelEditor::LevelEditor()
    : gridWidth(50), gridHeight(30), tileSize(32.0f), currentTileType(TileType::WALL),
    isActive(false), showGrid(true), isPainting(false), isErasing(false),
    cameraPosition(0.0f, 0.0f), zoomLevel(1.0f), torchAnimationTime(0.0f)
{
    Initialize();
}

LevelEditor::~LevelEditor()
{
}

void LevelEditor::Initialize()
{
    // Initialize grid
    NewLevel(gridWidth, gridHeight);

    // Setup view
    editorView.setSize(800.0f, 600.0f);
    editorView.setCenter(0.0f, 0.0f);

    // Initialize tile information
    InitializeTileInfos();

    // Load textures
    LoadTextures();

    // Setup UI
    SetupUI();
}

void LevelEditor::InitializeTileInfos()
{
    tileInfos.clear();

    // Basic tiles
    tileInfos.push_back(TileInfo(TileType::EMPTY, false, false, sf::Vector2i(0, 0), ""));
    tileInfos.push_back(TileInfo(TileType::WALL, true, false, sf::Vector2i(0, 0), "wall"));
    tileInfos.push_back(TileInfo(TileType::PLATFORM, true, false, sf::Vector2i(0, 0), "platform"));
    tileInfos.push_back(TileInfo(TileType::SPIKES, true, false, sf::Vector2i(0, 0), "spikes"));
    tileInfos.push_back(TileInfo(TileType::TORCH, false, true, sf::Vector2i(0, 0), "torch"));
    tileInfos.push_back(TileInfo(TileType::DOOR, true, false, sf::Vector2i(0, 0), "door"));

    // Tileset blocks - assuming 16x16 tiles in a 4x4 grid
    tileInfos.push_back(TileInfo(TileType::BLOCK_TOP_LEFT, true, false, sf::Vector2i(0, 0), "tileset"));
    tileInfos.push_back(TileInfo(TileType::BLOCK_TOP, true, false, sf::Vector2i(1, 0), "tileset"));
    tileInfos.push_back(TileInfo(TileType::BLOCK_TOP_RIGHT, true, false, sf::Vector2i(2, 0), "tileset"));
    tileInfos.push_back(TileInfo(TileType::BLOCK_LEFT, true, false, sf::Vector2i(0, 1), "tileset"));
    tileInfos.push_back(TileInfo(TileType::BLOCK_CENTER, true, false, sf::Vector2i(1, 1), "tileset"));
    tileInfos.push_back(TileInfo(TileType::BLOCK_RIGHT, true, false, sf::Vector2i(2, 1), "tileset"));
    tileInfos.push_back(TileInfo(TileType::BLOCK_BOTTOM_LEFT, true, false, sf::Vector2i(0, 2), "tileset"));
    tileInfos.push_back(TileInfo(TileType::BLOCK_BOTTOM, true, false, sf::Vector2i(1, 2), "tileset"));
    tileInfos.push_back(TileInfo(TileType::BLOCK_BOTTOM_RIGHT, true, false, sf::Vector2i(2, 2), "tileset"));

    // Shadow variations
    tileInfos.push_back(TileInfo(TileType::BLOCK_SHADOW_LEFT, false, false, sf::Vector2i(3, 0), "tileset"));
    tileInfos.push_back(TileInfo(TileType::BLOCK_SHADOW_RIGHT, false, false, sf::Vector2i(3, 1), "tileset"));
    tileInfos.push_back(TileInfo(TileType::BLOCK_SHADOW_BOTTOM, false, false, sf::Vector2i(3, 2), "tileset"));
    tileInfos.push_back(TileInfo(TileType::BLOCK_SHADOW_CORNER, false, false, sf::Vector2i(3, 3), "tileset"));
}

void LevelEditor::LoadTextures()
{
    // Load individual textures
    sf::Texture spikesTexture;
    if (spikesTexture.loadFromFile("assets/spikes.png")) {
        Resources::textures["spikes"] = spikesTexture;
    }

    sf::Texture torchTexture;
    if (torchTexture.loadFromFile("assets/torch.png")) {
        Resources::textures["torch"] = torchTexture;
    }

    sf::Texture tilesetTexture;
    if (tilesetTexture.loadFromFile("assets/tileset.png")) {
        Resources::textures["tileset"] = tilesetTexture;
    }

    sf::Texture doorTexture;
    if (doorTexture.loadFromFile("assets/door.png")) {
        Resources::textures["door"] = doorTexture;
    }

    // Create placeholder platform texture (to be replaced later with actual sprite)
    sf::Texture platformTexture;
    sf::Image platformImage;
    platformImage.create(32, 8, sf::Color::Red);
    platformTexture.loadFromImage(platformImage);
    Resources::textures["platform"] = platformTexture;

    // Create basic wall texture
    sf::Texture wallTexture;
    sf::Image wallImage;
    wallImage.create(32, 32, sf::Color(100, 100, 100));
    wallTexture.loadFromImage(wallImage);
    Resources::textures["wall"] = wallTexture;
}

void LevelEditor::SetupUI()
{
    // Load font for UI
    if (!font.loadFromFile("assets/font.ttf")) {
        // Use default font if custom font fails to load
    }

    // Setup info text
    infoText.setFont(font);
    infoText.setCharacterSize(16);
    infoText.setFillColor(sf::Color::White);
    infoText.setPosition(10, 10);

    // Setup selected tile indicator
    selectedTileIndicator.setSize(sf::Vector2f(tileSize, tileSize));
    selectedTileIndicator.setFillColor(sf::Color::Transparent);
    selectedTileIndicator.setOutlineColor(sf::Color::Yellow);
    selectedTileIndicator.setOutlineThickness(2.0f);
}

void LevelEditor::HandleInput(const sf::Event& event, const sf::RenderWindow& window)
{
    if (!isActive) return;

    if (event.type == sf::Event::KeyPressed) {
        switch (event.key.code) {
        case sf::Keyboard::Num1: SetCurrentTile(TileType::WALL); break;
        case sf::Keyboard::Num2: SetCurrentTile(TileType::PLATFORM); break;
        case sf::Keyboard::Num3: SetCurrentTile(TileType::SPIKES); break;
        case sf::Keyboard::Num4: SetCurrentTile(TileType::TORCH); break;
        case sf::Keyboard::Num5: SetCurrentTile(TileType::DOOR); break;
        case sf::Keyboard::Q: SetCurrentTile(TileType::BLOCK_TOP_LEFT); break;
        case sf::Keyboard::W: SetCurrentTile(TileType::BLOCK_TOP); break;
        case sf::Keyboard::E: SetCurrentTile(TileType::BLOCK_TOP_RIGHT); break;
        case sf::Keyboard::A: SetCurrentTile(TileType::BLOCK_LEFT); break;
        case sf::Keyboard::S: SetCurrentTile(TileType::BLOCK_CENTER); break;
        case sf::Keyboard::D: SetCurrentTile(TileType::BLOCK_RIGHT); break;
        case sf::Keyboard::Z: SetCurrentTile(TileType::BLOCK_BOTTOM_LEFT); break;
        case sf::Keyboard::X: SetCurrentTile(TileType::BLOCK_BOTTOM); break;
        case sf::Keyboard::C: SetCurrentTile(TileType::BLOCK_BOTTOM_RIGHT); break;
        case sf::Keyboard::G: showGrid = !showGrid; break;
        case sf::Keyboard::F1: SaveLevel("level.txt"); break;
        case sf::Keyboard::F2: LoadLevel("level.txt"); break;
        case sf::Keyboard::F3: ExportAsImage("level.png"); break;
        case sf::Keyboard::Escape: isActive = false; break;
        }
    }

    if (event.type == sf::Event::MouseButtonPressed) {
        if (event.mouseButton.button == sf::Mouse::Left) {
            isPainting = true;
            sf::Vector2f mouseWorldPos = window.mapPixelToCoords(sf::Mouse::getPosition(window), editorView);
            sf::Vector2i gridPos = GetGridPosition(mouseWorldPos);
            if (gridPos.x >= 0 && gridPos.x < gridWidth && gridPos.y >= 0 && gridPos.y < gridHeight) {
                PlaceTile(gridPos.x, gridPos.y);
            }
        }
        else if (event.mouseButton.button == sf::Mouse::Right) {
            isErasing = true;
            sf::Vector2f mouseWorldPos = window.mapPixelToCoords(sf::Mouse::getPosition(window), editorView);
            sf::Vector2i gridPos = GetGridPosition(mouseWorldPos);
            if (gridPos.x >= 0 && gridPos.x < gridWidth && gridPos.y >= 0 && gridPos.y < gridHeight) {
                RemoveTile(gridPos.x, gridPos.y);
            }
        }
    }

    if (event.type == sf::Event::MouseButtonReleased) {
        if (event.mouseButton.button == sf::Mouse::Left) {
            isPainting = false;
        }
        else if (event.mouseButton.button == sf::Mouse::Right) {
            isErasing = false;
        }
    }

    if (event.type == sf::Event::MouseMoved && (isPainting || isErasing)) {
        sf::Vector2f mouseWorldPos = window.mapPixelToCoords(sf::Mouse::getPosition(window), editorView);
        sf::Vector2i gridPos = GetGridPosition(mouseWorldPos);
        if (gridPos.x >= 0 && gridPos.x < gridWidth && gridPos.y >= 0 && gridPos.y < gridHeight) {
            if (isPainting) {
                PlaceTile(gridPos.x, gridPos.y);
            }
            else if (isErasing) {
                RemoveTile(gridPos.x, gridPos.y);
            }
        }
    }

    // Camera movement
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
        cameraPosition.x -= 200.0f * 0.016f; // Assuming 60 FPS
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
        cameraPosition.x += 200.0f * 0.016f;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
        cameraPosition.y -= 200.0f * 0.016f;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
        cameraPosition.y += 200.0f * 0.016f;
    }
}

void LevelEditor::Update(float deltaTime)
{
    if (!isActive) return;

    // Update torch animation
    torchAnimationTime += deltaTime;
    if (torchAnimationTime > 8.0f) { // 8 frames * some time per frame
        torchAnimationTime = 0.0f;
    }

    // Update camera
    UpdateCamera();
}

void LevelEditor::UpdateCamera()
{
    editorView.setCenter(cameraPosition);
}

void LevelEditor::Render(sf::RenderWindow& window)
{
    if (!isActive) return;

    window.setView(editorView);

    // Render grid
    if (showGrid) {
        RenderGrid(window);
    }

    // Render tiles
    for (int y = 0; y < gridHeight; ++y) {
        for (int x = 0; x < gridWidth; ++x) {
            if (grid[y][x] != TileType::EMPTY) {
                sf::Vector2f worldPos = GetWorldPosition(x, y);
                RenderTile(window, grid[y][x], worldPos);
            }
        }
    }

    // Render selected tile indicator at mouse position
    sf::Vector2f mouseWorldPos = window.mapPixelToCoords(sf::Mouse::getPosition(window), editorView);
    sf::Vector2i gridPos = GetGridPosition(mouseWorldPos);
    if (gridPos.x >= 0 && gridPos.x < gridWidth && gridPos.y >= 0 && gridPos.y < gridHeight) {
        sf::Vector2f worldPos = GetWorldPosition(gridPos.x, gridPos.y);
        selectedTileIndicator.setPosition(worldPos.x - tileSize / 2, worldPos.y - tileSize / 2);
        window.draw(selectedTileIndicator);
    }

    // Reset to default view for UI
    window.setView(window.getDefaultView());
    RenderUI(window);
}

void LevelEditor::RenderGrid(sf::RenderWindow& window)
{
    sf::VertexArray lines(sf::Lines);
    sf::Color gridColor(255, 255, 255, 50);

    // Vertical lines
    for (int x = 0; x <= gridWidth; ++x) {
        float worldX = x * tileSize - (gridWidth * tileSize) / 2.0f;
        lines.append(sf::Vertex(sf::Vector2f(worldX, -(gridHeight * tileSize) / 2.0f), gridColor));
        lines.append(sf::Vertex(sf::Vector2f(worldX, (gridHeight * tileSize) / 2.0f), gridColor));
    }

    // Horizontal lines
    for (int y = 0; y <= gridHeight; ++y) {
        float worldY = y * tileSize - (gridHeight * tileSize) / 2.0f;
        lines.append(sf::Vertex(sf::Vector2f(-(gridWidth * tileSize) / 2.0f, worldY), gridColor));
        lines.append(sf::Vertex(sf::Vector2f((gridWidth * tileSize) / 2.0f, worldY), gridColor));
    }

    window.draw(lines);
}

void LevelEditor::RenderUI(sf::RenderWindow& window)
{
    // Update info text
    std::string info = "Level Editor - Current Tile: ";
    switch (currentTileType) {
    case TileType::WALL: info += "Wall (1)"; break;
    case TileType::PLATFORM: info += "Platform (2)"; break;
    case TileType::SPIKES: info += "Spikes (3)"; break;
    case TileType::TORCH: info += "Torch (4)"; break;
    case TileType::DOOR: info += "Door (5)"; break;
    case TileType::BLOCK_TOP_LEFT: info += "Block Top-Left (Q)"; break;
    case TileType::BLOCK_TOP: info += "Block Top (W)"; break;
    case TileType::BLOCK_TOP_RIGHT: info += "Block Top-Right (E)"; break;
    case TileType::BLOCK_LEFT: info += "Block Left (A)"; break;
    case TileType::BLOCK_CENTER: info += "Block Center (S)"; break;
    case TileType::BLOCK_RIGHT: info += "Block Right (D)"; break;
    case TileType::BLOCK_BOTTOM_LEFT: info += "Block Bottom-Left (Z)"; break;
    case TileType::BLOCK_BOTTOM: info += "Block Bottom (X)"; break;
    case TileType::BLOCK_BOTTOM_RIGHT: info += "Block Bottom-Right (C)"; break;
    default: info += "Unknown"; break;
    }
    info += "\nControls: Left Click - Place, Right Click - Erase, Arrow Keys - Move Camera";
    info += "\nF1 - Save, F2 - Load, F3 - Export, G - Toggle Grid, ESC - Exit Editor";

    infoText.setString(info);
    window.draw(infoText);
}

void LevelEditor::RenderTile(sf::RenderWindow& window, TileType type, const sf::Vector2f& position)
{
    if (type == TileType::TORCH) {
        RenderTorch(window, position, torchAnimationTime);
        return;
    }

    for (const auto& tileInfo : tileInfos) {
        if (tileInfo.type == type && !tileInfo.textureName.empty()) {
            auto it = Resources::textures.find(tileInfo.textureName);
            if (it != Resources::textures.end()) {
                sf::Sprite sprite;
                sprite.setTexture(it->second);

                if (tileInfo.textureName == "tileset") {
                    // Set texture rect for tileset tiles
                    sf::IntRect rect = GetTilesetRect(type);
                    sprite.setTextureRect(rect);
                }

                sprite.setOrigin(sprite.getLocalBounds().width / 2.0f, sprite.getLocalBounds().height / 2.0f);
                sprite.setPosition(position);

                // Scale to fit tile size
                float scaleX = tileSize / sprite.getLocalBounds().width;
                float scaleY = tileSize / sprite.getLocalBounds().height;
                sprite.setScale(scaleX, scaleY);

                window.draw(sprite);
            }
            break;
        }
    }
}

void LevelEditor::RenderTorch(sf::RenderWindow& window, const sf::Vector2f& position, float animTime)
{
    auto it = Resources::textures.find("torch");
    if (it != Resources::textures.end()) {
        sf::Sprite sprite;
        sprite.setTexture(it->second);

        // Calculate animation frame (assuming 8 frames horizontally)
        int frameWidth = it->second.getSize().x / 8;
        int frameHeight = it->second.getSize().y;
        int currentFrame = (int)(animTime * 8.0f) % 8; // 8 frames animation

        sf::IntRect frameRect(currentFrame * frameWidth, 0, frameWidth, frameHeight);
        sprite.setTextureRect(frameRect);

        sprite.setOrigin(frameWidth / 2.0f, frameHeight / 2.0f);
        sprite.setPosition(position);

        // Scale to fit tile size
        float scaleX = tileSize / frameWidth;
        float scaleY = tileSize / frameHeight;
        sprite.setScale(scaleX, scaleY);

        window.draw(sprite);
    }
}

sf::IntRect LevelEditor::GetTilesetRect(TileType type) const
{
    for (const auto& tileInfo : tileInfos) {
        if (tileInfo.type == type) {
            // Assuming 16x16 tiles in tileset
            int tileWidth = 16;
            int tileHeight = 16;
            return sf::IntRect(
                tileInfo.textureCoord.x * tileWidth,
                tileInfo.textureCoord.y * tileHeight,
                tileWidth,
                tileHeight
            );
        }
    }
    return sf::IntRect(0, 0, 16, 16);
}

void LevelEditor::SetCurrentTile(TileType type)
{
    currentTileType = type;
}

void LevelEditor::PlaceTile(int x, int y)
{
    if (x >= 0 && x < gridWidth && y >= 0 && y < gridHeight) {
        grid[y][x] = currentTileType;
    }
}

void LevelEditor::RemoveTile(int x, int y)
{
    if (x >= 0 && x < gridWidth && y >= 0 && y < gridHeight) {
        grid[y][x] = TileType::EMPTY;
    }
}

TileType LevelEditor::GetTileAt(int x, int y) const
{
    if (x >= 0 && x < gridWidth && y >= 0 && y < gridHeight) {
        return grid[y][x];
    }
    return TileType::EMPTY;
}

bool LevelEditor::HasCollisionAt(int x, int y) const
{
    TileType type = GetTileAt(x, y);
    for (const auto& tileInfo : tileInfos) {
        if (tileInfo.type == type) {
            return tileInfo.hasCollision;
        }
    }
    return false;
}

sf::Vector2i LevelEditor::GetGridPosition(const sf::Vector2f& worldPos) const
{
    int x = (int)((worldPos.x + (gridWidth * tileSize) / 2.0f) / tileSize);
    int y = (int)((worldPos.y + (gridHeight * tileSize) / 2.0f) / tileSize);
    return sf::Vector2i(x, y);
}

sf::Vector2f LevelEditor::GetWorldPosition(int gridX, int gridY) const
{
    float x = gridX * tileSize - (gridWidth * tileSize) / 2.0f + tileSize / 2.0f;
    float y = gridY * tileSize - (gridHeight * tileSize) / 2.0f + tileSize / 2.0f;
    return sf::Vector2f(x, y);
}

void LevelEditor::NewLevel(int width, int height)
{
    gridWidth = width;
    gridHeight = height;
    grid.clear();
    grid.resize(height, std::vector<TileType>(width, TileType::EMPTY));
}

void LevelEditor::SaveLevel(const std::string& filename)
{
    std::ofstream file(filename);
    if (file.is_open()) {
        file << gridWidth << " " << gridHeight << std::endl;
        for (int y = 0; y < gridHeight; ++y) {
            for (int x = 0; x < gridWidth; ++x) {
                file << (int)grid[y][x];
                if (x < gridWidth - 1) file << " ";
            }
            file << std::endl;
        }
        file.close();
    }
}

void LevelEditor::LoadLevel(const std::string& filename)
{
    std::ifstream file(filename);
    if (file.is_open()) {
        file >> gridWidth >> gridHeight;
        NewLevel(gridWidth, gridHeight);

        for (int y = 0; y < gridHeight; ++y) {
            for (int x = 0; x < gridWidth; ++x) {
                int tileValue;
                file >> tileValue;
                grid[y][x] = (TileType)tileValue;
            }
        }
        file.close();
    }
}

void LevelEditor::ExportAsImage(const std::string& filename)
{
    sf::Image image;
    image.create(gridWidth, gridHeight);

    for (int y = 0; y < gridHeight; ++y) {
        for (int x = 0; x < gridWidth; ++x) {
            sf::Color pixelColor;
            switch (grid[y][x]) {
            case TileType::EMPTY: pixelColor = sf::Color::Black; break;
            case TileType::WALL: pixelColor = sf::Color::White; break;
            case TileType::PLATFORM: pixelColor = sf::Color::Red; break;
            case TileType::SPIKES: pixelColor = sf::Color::Red; break;
            case TileType::TORCH: pixelColor = sf::Color::Yellow; break;
            case TileType::DOOR: pixelColor = sf::Color::Blue; break;
            default: pixelColor = sf::Color::Green; break;
            }
            image.setPixel(x, y, pixelColor);
        }
    }

    image.saveToFile(filename);
}