#include "stdafx.h"
#include "LevelEditor.h"
#include "TileRenderer.h"
#include "Map.h"
#include "Resources.h"
#include "Camera.h"

extern Camera camera;

LevelEditor::~LevelEditor()
{
}

LevelEditor::LevelEditor()
    : gridWidth(50), gridHeight(30), tileSize(32.0f), currentTileType(TileType::WALL),
    isActive(false), showGrid(true), isPainting(false), isErasing(false),
    cameraPosition(0.0f, 0.0f), zoomLevel(1.0f), torchAnimationTime(0.0f)
{
    // Crear el renderer compartido
    tileRenderer = std::make_shared<TileRenderer>(tileSize);
    Initialize();
}

// Simplifica InitializeTileInfos() - ahora se hace en TileRenderer
void LevelEditor::Initialize()
{
    // Initialize grid
    NewLevel(gridWidth, gridHeight);

    // Setup view
    editorView.setSize(800.0f, 600.0f);
    editorView.setCenter(0.0f, 0.0f);

    // TileRenderer ya maneja la inicialización de tile infos

    // Load textures - esto se puede mover también al TileRenderer
    LoadTextures();

    // Setup UI
    SetupUI();
}

void LevelEditor::LoadTextures()
{
    // Estructura para definir texturas a cargar
    struct TextureInfo {
        std::string name;
        std::string path;
        bool required;
    };

    std::vector<TextureInfo> texturesToLoad = {
        {"spikes", "assets/sprites/spike.png", false},
        {"torch", "assets/sprites/torch.png", false},
        {"tileset", "assets/sprites/tileset.png", false},
        {"door", "assets/sprites/door.png", false}
    };

    // Cargar texturas con mejor manejo de errores
    for (const auto& textureInfo : texturesToLoad) {
        if (!Resources::LoadTexture(textureInfo.name, textureInfo.path)) {
            std::cout << "Warning: Could not load " << textureInfo.name
                << " from " << textureInfo.path << std::endl;
            if (textureInfo.required) {
                std::cout << "Error: Required texture failed to load!" << std::endl;
            }
        }
    }
    CreatePlaceholderTextures();
}

void LevelEditor::CreatePlaceholderTextures()
{
	// Platform texture (mejorado)
	if (!Resources::GetTexture("platform"))
	{
		sf::Texture platformTexture;
		sf::Image platformImage;
		platformImage.create(64, 16); // Más ancho que alto para plataformas

		// Crear patrón de madera más realista
		sf::Color woodBase(139, 69, 19);
		sf::Color woodLight(160, 82, 45);
		sf::Color woodDark(101, 67, 33);

		for (int x = 0; x < 64; ++x)
		{
			for (int y = 0; y < 16; ++y)
			{
				sf::Color pixelColor = woodBase;

				// Añadir variación
				if (y == 0 || y == 1) pixelColor = woodLight; // Top highlight
				if (y >= 14) pixelColor = woodDark; // Bottom shadow
				if (x % 8 == 0) pixelColor = woodDark; // Vertical lines

				platformImage.setPixel(x, y, pixelColor);
			}
		}

		platformTexture.loadFromImage(platformImage);
		Resources::AddTexture("platform", platformTexture);
	}

	// Wall texture (mejorado)
	if (!Resources::GetTexture("wall"))
	{
		sf::Texture wallTexture;
		sf::Image wallImage;
		wallImage.create(32, 32);

		// Crear patrón de piedra
		sf::Color stoneBase(100, 100, 100);
		sf::Color stoneLight(130, 130, 130);
		sf::Color stoneDark(70, 70, 70);

		for (int x = 0; x < 32; ++x)
		{
			for (int y = 0; y < 32; ++y)
			{
				sf::Color pixelColor = stoneBase;

				// Crear patrón de bloques de piedra
				if ((x + y) % 8 < 2) pixelColor = stoneDark;
				if (x % 16 == 0 || y % 16 == 0) pixelColor = stoneLight;

				wallImage.setPixel(x, y, pixelColor);
			}
		}

		wallTexture.loadFromImage(wallImage);
		Resources::AddTexture("wall", wallTexture);
	}
}

std::string LevelEditor::GetTextureNameForTileType(TileType type) const
{
    for (const auto& tileInfo : tileInfos) {
        if (tileInfo.type == type) {
            return tileInfo.textureName;
        }
    }
    return "";
}

void LevelEditor::SetupUI()
{
    // Load font for UI
    if (!font.loadFromFile("assets/arial.ttf")) {
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

// Fixed LevelEditor methods

void LevelEditor::RenderTile(sf::RenderWindow& window, TileType type, const sf::Vector2f& position)
{
    tileRenderer->RenderTile(window, type, position, torchAnimationTime);
}

sf::IntRect LevelEditor::GetTilesetRect(TileType type) const
{
    return tileRenderer->GetTilesetRect(type);
}

void LevelEditor::SetCurrentTile(TileType type)
{
    currentTileType = type;
}

// Usa este método en lugar de verificar manualmente:
void LevelEditor::PlaceTile(int x, int y)
{
    if (IsValidGridPosition(x, y)) {
        grid[y][x] = currentTileType;
    }
}

void LevelEditor::RemoveTile(int x, int y)
{
    if (IsValidGridPosition(x, y)) {
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

bool LevelEditor::IsValidGridPosition(int x, int y) const
{
	return x >= 0 && x < gridWidth && y >= 0 && y < gridHeight;
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
    if (!file.is_open()) {
        std::cout << "Error: Could not open file for saving: " << filename << std::endl;
        return;
    }

    try {
        file << gridWidth << " " << gridHeight << std::endl;
        for (int y = 0; y < gridHeight; ++y) {
            for (int x = 0; x < gridWidth; ++x) {
                file << (int)grid[y][x];
                if (x < gridWidth - 1) file << " ";
            }
            file << std::endl;
        }
        file.close();
        std::cout << "Level saved successfully: " << filename << std::endl;
    }
    catch (const std::exception& e) {
        std::cout << "Error saving level: " << e.what() << std::endl;
    }
}

void LevelEditor::LoadLevel(const std::string& filename)
{
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cout << "Error: Could not open file for loading: " << filename << std::endl;
        return;
    }

    try {
        int newWidth, newHeight;
        file >> newWidth >> newHeight;

        if (newWidth <= 0 || newHeight <= 0 || newWidth > 200 || newHeight > 200) {
            std::cout << "Error: Invalid level dimensions: " << newWidth << "x" << newHeight << std::endl;
            file.close();
            return;
        }

        NewLevel(newWidth, newHeight);

        for (int y = 0; y < gridHeight; ++y) {
            for (int x = 0; x < gridWidth; ++x) {
                int tileValue;
                if (!(file >> tileValue)) {
                    std::cout << "Error: Invalid tile data at position " << x << "," << y << std::endl;
                    file.close();
                    return;
                }
                grid[y][x] = (TileType)tileValue;
            }
        }
        file.close();
        std::cout << "Level loaded successfully: " << filename << std::endl;
    }
    catch (const std::exception& e) {
        std::cout << "Error loading level: " << e.what() << std::endl;
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