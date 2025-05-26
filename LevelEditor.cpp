#include "stdafx.h"
#include "LevelEditor.h"
#include "Resources.h"
#include "Game.h"

LevelEditor::LevelEditor(Map& map, float cellSize)
    : mapRef(map), cellSize(cellSize), editorMode(false),
    selectedTileType(TileType::SOLID_BLOCK), isPlacing(false),
    isErasing(false), lastMouseGrid(-1, -1)
{
    InitUI();
}

void LevelEditor::InitUI()
{
    // Cargar fuente (asume que tienes una fuente disponible)
    // font.loadFromFile("assets/fonts/arial.ttf");

    instructionText.setFont(font);
    instructionText.setCharacterSize(16);
    instructionText.setFillColor(sf::Color::White);
    instructionText.setString("Level Editor\n[1-9] Select Tile | [LMB] Place | [RMB] Erase | [C] Clear | [S] Save | [L] Load | [ESC] Exit");
    instructionText.setPosition(10, 10);

    tileSelector.setSize(sf::Vector2f(200, 30));
    tileSelector.setFillColor(sf::Color(0, 0, 0, 180));
    tileSelector.setOutlineColor(sf::Color::White);
    tileSelector.setOutlineThickness(2);
    tileSelector.setPosition(10, 100);

    tileTypeText.setFont(font);
    tileTypeText.setCharacterSize(14);
    tileTypeText.setFillColor(sf::Color::White);
    tileTypeText.setPosition(15, 105);

    tileInfoText.setFont(font);
    tileInfoText.setCharacterSize(12);
    tileInfoText.setFillColor(sf::Color::Yellow);
    tileInfoText.setPosition(15, 140);
}

void LevelEditor::HandleInput(const sf::Event& event, const sf::RenderWindow& window)
{
    if (!editorMode) return;

    if (event.type == sf::Event::KeyPressed)
    {
        switch (event.key.code)
        {
        case sf::Keyboard::Num1: SetSelectedTile(TileType::EMPTY); break;
        case sf::Keyboard::Num2: SetSelectedTile(TileType::SOLID_BLOCK); break;
        case sf::Keyboard::Num3: SetSelectedTile(TileType::SPIKE); break;
        case sf::Keyboard::Num4: SetSelectedTile(TileType::DOOR); break;
        case sf::Keyboard::Num5: SetSelectedTile(TileType::TORCH); break;
        case sf::Keyboard::Num6: SetSelectedTile(TileType::PLATFORM); break;
        case sf::Keyboard::Num7: SetSelectedTile(TileType::BLOCK_TOP_LEFT); break;
        case sf::Keyboard::Num8: SetSelectedTile(TileType::BLOCK_TOP); break;
        case sf::Keyboard::Num9: SetSelectedTile(TileType::BLOCK_TOP_RIGHT); break;
        case sf::Keyboard::Q: SetSelectedTile(TileType::BLOCK_LEFT); break;
        case sf::Keyboard::W: SetSelectedTile(TileType::BLOCK_CENTER); break;
        case sf::Keyboard::E: SetSelectedTile(TileType::BLOCK_RIGHT); break;
        case sf::Keyboard::A: SetSelectedTile(TileType::BLOCK_BOTTOM_LEFT); break;
        case sf::Keyboard::S: SetSelectedTile(TileType::BLOCK_BOTTOM); break;
        case sf::Keyboard::D: SetSelectedTile(TileType::BLOCK_BOTTOM_RIGHT); break;
        case sf::Keyboard::Z: SetSelectedTile(TileType::BLOCK_SHADOW_TOP); break;
        case sf::Keyboard::X: SetSelectedTile(TileType::BLOCK_SHADOW_LEFT); break;
        case sf::Keyboard::C: SetSelectedTile(TileType::BLOCK_SHADOW_RIGHT); break;
        case sf::Keyboard::V: SetSelectedTile(TileType::BLOCK_SHADOW_BOTTOM); break;
        case sf::Keyboard::F5: SaveLevel("level.dat"); break;
        case sf::Keyboard::F9: LoadLevel("level.dat"); break;
        case sf::Keyboard::Delete: ClearLevel(); break;
        case sf::Keyboard::Escape: SetEditorMode(false); break;
        }
    }

    if (event.type == sf::Event::MouseButtonPressed)
    {
        sf::Vector2i mousePos = sf::Mouse::getPosition(window);
        sf::Vector2i gridPos = ScreenToGrid(mousePos, window);

        if (event.mouseButton.button == sf::Mouse::Left)
        {
            isPlacing = true;
            PlaceTile(gridPos);
        }
        else if (event.mouseButton.button == sf::Mouse::Right)
        {
            isErasing = true;
            EraseTile(gridPos);
        }
        lastMouseGrid = gridPos;
    }

    if (event.type == sf::Event::MouseButtonReleased)
    {
        if (event.mouseButton.button == sf::Mouse::Left)
            isPlacing = false;
        else if (event.mouseButton.button == sf::Mouse::Right)
            isErasing = false;
    }

    if (event.type == sf::Event::MouseMoved && (isPlacing || isErasing))
    {
        sf::Vector2i mousePos = sf::Mouse::getPosition(window);
        sf::Vector2i gridPos = ScreenToGrid(mousePos, window);

        if (gridPos != lastMouseGrid)
        {
            if (isPlacing)
                PlaceTile(gridPos);
            else if (isErasing)
                EraseTile(gridPos);
            lastMouseGrid = gridPos;
        }
    }
}

void LevelEditor::Update(float deltaTime)
{
    if (!editorMode) return;

    UpdateTileSelection();
}

void LevelEditor::UpdateTileSelection()
{
    tileTypeText.setString("Selected: " + TileTypeToString(selectedTileType));

    std::string info = "";
    if (IsSolidTile(selectedTileType))
        info += "Solid ";
    if (IsPlatformTile(selectedTileType))
        info += "Platform ";
    if (IsDamageTile(selectedTileType))
        info += "Damage ";
    if (IsAnimatedTile(selectedTileType))
        info += "Animated ";

    tileInfoText.setString("Properties: " + info);
}

void LevelEditor::Draw(Renderer& renderer)
{
    if (!editorMode) return;

    DrawGrid(renderer, camera.getView(sf::Vector2f(800, 600)));
    DrawTilePreview(renderer);
}

void LevelEditor::DrawUI(sf::RenderWindow& window)
{
    if (!editorMode) return;

    window.draw(instructionText);
    window.draw(tileSelector);
    window.draw(tileTypeText);
    window.draw(tileInfoText);
}

void LevelEditor::DrawGrid(Renderer& renderer, const sf::View& view)
{
    // Implementar dibujado de grid basado en la vista de la cámara
    sf::Vector2f viewCenter = view.getCenter();
    sf::Vector2f viewSize = view.getSize();

    // Calcular los límites visibles del grid
    int startX = static_cast<int>((viewCenter.x - viewSize.x / 2) / cellSize);
    int endX = static_cast<int>((viewCenter.x + viewSize.x / 2) / cellSize) + 1;
    int startY = static_cast<int>((viewCenter.y - viewSize.y / 2) / cellSize);
    int endY = static_cast<int>((viewCenter.y + viewSize.y / 2) / cellSize) + 1;

    // Asegurar que están dentro de los límites del mapa
    startX = std::max(0, startX);
    endX = std::min(static_cast<int>(mapRef.grid[0].size()), endX);
    startY = std::max(0, startY);
    endY = std::min(static_cast<int>(mapRef.grid.size()), endY);

    // Dibujar líneas del grid (esto requeriría una implementación específica en tu renderer)
}

void LevelEditor::DrawTilePreview(Renderer& renderer)
{
    // Mostrar preview del tile seleccionado en la posición del mouse
    // Esto requeriría obtener la posición del mouse en coordenadas del mundo
}

sf::Vector2i LevelEditor::ScreenToGrid(const sf::Vector2i& screenPos, const sf::RenderWindow& window)
{
    sf::Vector2f worldPos = window.mapPixelToCoords(screenPos, camera.getView(sf::Vector2f(window.getSize())));
    return sf::Vector2i(static_cast<int>(worldPos.x / cellSize), static_cast<int>(worldPos.y / cellSize));
}

sf::Vector2f LevelEditor::GridToScreen(const sf::Vector2i& gridPos)
{
    return sf::Vector2f(gridPos.x * cellSize + cellSize / 2, gridPos.y * cellSize + cellSize / 2);
}

void LevelEditor::PlaceTile(const sf::Vector2i& gridPos)
{
    if (gridPos.x >= 0 && gridPos.x < static_cast<int>(mapRef.grid[0].size()) &&
        gridPos.y >= 0 && gridPos.y < static_cast<int>(mapRef.grid.size()))
    {
        mapRef.grid[gridPos.y][gridPos.x] = static_cast<int>(selectedTileType);
    }
}

void LevelEditor::EraseTile(const sf::Vector2i& gridPos)
{
    if (gridPos.x >= 0 && gridPos.x < static_cast<int>(mapRef.grid[0].size()) &&
        gridPos.y >= 0 && gridPos.y < static_cast<int>(mapRef.grid.size()))
    {
        mapRef.grid[gridPos.y][gridPos.x] = static_cast<int>(TileType::EMPTY);
    }
}

void LevelEditor::SetSelectedTile(TileType type)
{
    selectedTileType = type;
}

std::string LevelEditor::TileTypeToString(TileType type)
{
    switch (type)
    {
    case TileType::EMPTY: return "Empty";
    case TileType::SOLID_BLOCK: return "Solid Block";
    case TileType::SPIKE: return "Spike";
    case TileType::DOOR: return "Door";
    case TileType::TORCH: return "Torch";
    case TileType::PLATFORM: return "Platform";
    case TileType::BLOCK_TOP_LEFT: return "Block Top-Left";
    case TileType::BLOCK_TOP: return "Block Top";
    case TileType::BLOCK_TOP_RIGHT: return "Block Top-Right";
    case TileType::BLOCK_LEFT: return "Block Left";
    case TileType::BLOCK_CENTER: return "Block Center";
    case TileType::BLOCK_RIGHT: return "Block Right";
    case TileType::BLOCK_BOTTOM_LEFT: return "Block Bottom-Left";
    case TileType::BLOCK_BOTTOM: return "Block Bottom";
    case TileType::BLOCK_BOTTOM_RIGHT: return "Block Bottom-Right";
    case TileType::BLOCK_SHADOW_TOP: return "Shadow Top";
    case TileType::BLOCK_SHADOW_LEFT: return "Shadow Left";
    case TileType::BLOCK_SHADOW_RIGHT: return "Shadow Right";
    case TileType::BLOCK_SHADOW_BOTTOM: return "Shadow Bottom";
    default: return "Unknown";
    }
}

// Funciones estáticas para determinar propiedades de los tiles
bool LevelEditor::IsSolidTile(TileType type)
{
    switch (type)
    {
    case TileType::SOLID_BLOCK:
    case TileType::BLOCK_TOP_LEFT:
    case TileType::BLOCK_TOP:
    case TileType::BLOCK_TOP_RIGHT:
    case TileType::BLOCK_LEFT:
    case TileType::BLOCK_CENTER:
    case TileType::BLOCK_RIGHT:
    case TileType::BLOCK_BOTTOM_LEFT:
    case TileType::BLOCK_BOTTOM:
    case TileType::BLOCK_BOTTOM_RIGHT:
    case TileType::DOOR: // La puerta puede ser sólida dependiendo del estado
        return true;
    default:
        return false;
    }
}

bool LevelEditor::IsPlatformTile(TileType type)
{
    return type == TileType::PLATFORM;
}

bool LevelEditor::IsDamageTile(TileType type)
{
    return type == TileType::SPIKE;
}

bool LevelEditor::IsAnimatedTile(TileType type)
{
    switch (type)
    {
    case TileType::TORCH:
    case TileType::DOOR:
        return true;
    default:
        return false;
    }
}

void LevelEditor::SaveLevel(const std::string& filename)
{
    // Implementar guardado de nivel
    std::ofstream file(filename, std::ios::binary);
    if (file.is_open())
    {
        size_t height = mapRef.grid.size();
        size_t width = mapRef.grid[0].size();

        file.write(reinterpret_cast<const char*>(&width), sizeof(width));
        file.write(reinterpret_cast<const char*>(&height), sizeof(height));

        for (const auto& row : mapRef.grid)
        {
            file.write(reinterpret_cast<const char*>(row.data()), row.size() * sizeof(int));
        }
        file.close();
    }
}

void LevelEditor::LoadLevel(const std::string& filename)
{
    // Implementar carga de nivel
    std::ifstream file(filename, std::ios::binary);
    if (file.is_open())
    {
        size_t width, height;
        file.read(reinterpret_cast<char*>(&width), sizeof(width));
        file.read(reinterpret_cast<char*>(&height), sizeof(height));

        mapRef.createBoard(width, height);

        for (auto& row : mapRef.grid)
        {
            file.read(reinterpret_cast<char*>(row.data()), row.size() * sizeof(int));
        }
        file.close();
    }
}

void LevelEditor::ClearLevel()
{
    for (auto& row : mapRef.grid)
    {
        std::fill(row.begin(), row.end(), static_cast<int>(TileType::EMPTY));
    }
}

void LevelEditor::NewLevel(int width, int height)
{
    mapRef.createBoard(width, height);
    ClearLevel();
}