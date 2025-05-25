#include "stdafx.h"
#include "LevelEditor.h"
#include "Game.h"
#include "Resources.h"
#include <fstream>
#include <iostream>

LevelEditor::LevelEditor(Map& map, float cellSize)
    : mapRef(map), cellSize(cellSize), editorMode(false), selectedTileType(TileType::SOLID_BLOCK),
    isPlacing(false), isErasing(false), lastMouseGrid(-1, -1)
{
    InitUI();
}

void LevelEditor::InitUI()
{
    // Try to load a font (you might need to add a font file to your assets)
    if (!font.loadFromFile("./assets/arial.ttf")) {
        // If no font file, use default font
        std::cout << "Warning: Could not load font file. Using default font." << std::endl;
    }

    instructionText.setFont(font);
    instructionText.setCharacterSize(16);
    instructionText.setFillColor(sf::Color::White);
    instructionText.setString("Editor Mode:\n1-Solid 2-Spikes 3-Door\nLeft Click-Place Right Click-Erase\nE-Toggle Editor S-Save L-Load C-Clear N-New");

    tileTypeText.setFont(font);
    tileTypeText.setCharacterSize(18);
    tileTypeText.setFillColor(sf::Color::Yellow);

    tileSelector.setSize(sf::Vector2f(200, 30));
    tileSelector.setFillColor(sf::Color(0, 0, 0, 150));
    tileSelector.setOutlineThickness(2);
    tileSelector.setOutlineColor(sf::Color::White);
}

void LevelEditor::HandleInput(const sf::Event& event, const sf::RenderWindow& window)
{
    if (event.type == sf::Event::KeyPressed) {
        switch (event.key.code) {
        case sf::Keyboard::E:
            editorMode = !editorMode;
            break;
        case sf::Keyboard::Num1:
            if (editorMode) SetSelectedTile(TileType::SOLID_BLOCK);
            break;
        case sf::Keyboard::Num2:
            if (editorMode) SetSelectedTile(TileType::SPIKE);
            break;
        case sf::Keyboard::Num3:
            if (editorMode) SetSelectedTile(TileType::DOOR);
            break;
        case sf::Keyboard::S:
            if (editorMode) SaveLevel("level.dat");
            break;
        case sf::Keyboard::L:
            if (editorMode) LoadLevel("level.dat");
            break;
        case sf::Keyboard::C:
            if (editorMode) ClearLevel();
            break;
        case sf::Keyboard::N:
            if (editorMode) NewLevel(50, 50);
            break;
        }
    }

    if (!editorMode) return;

    if (event.type == sf::Event::MouseButtonPressed) {
        if (event.mouseButton.button == sf::Mouse::Left) {
            isPlacing = true;
            sf::Vector2i mousePos(event.mouseButton.x, event.mouseButton.y);
            sf::Vector2i gridPos = ScreenToGrid(mousePos, window.getView());
            PlaceTile(gridPos);
            lastMouseGrid = gridPos;
        }
        else if (event.mouseButton.button == sf::Mouse::Right) {
            isErasing = true;
            sf::Vector2i mousePos(event.mouseButton.x, event.mouseButton.y);
            sf::Vector2i gridPos = ScreenToGrid(mousePos, window.getView());
            EraseTile(gridPos);
            lastMouseGrid = gridPos;
        }
    }

    if (event.type == sf::Event::MouseButtonReleased) {
        if (event.mouseButton.button == sf::Mouse::Left) {
            isPlacing = false;
        }
        else if (event.mouseButton.button == sf::Mouse::Right) {
            isErasing = false;
        }
    }

    if (event.type == sf::Event::MouseMoved && (isPlacing || isErasing)) {
        sf::Vector2i mousePos(event.mouseMove.x, event.mouseMove.y);
        sf::Vector2i gridPos = ScreenToGrid(mousePos, window.getView());

        if (gridPos != lastMouseGrid) {
            if (isPlacing) {
                PlaceTile(gridPos);
            }
            else if (isErasing) {
                EraseTile(gridPos);
            }
            lastMouseGrid = gridPos;
        }
    }
}

void LevelEditor::Update(float deltaTime)
{
    // Update UI text
    tileTypeText.setString("Selected: " + TileTypeToString(selectedTileType));
}

void LevelEditor::Draw(Renderer& renderer)
{
    if (!editorMode) return;

    // Draw grid overlay
    DrawGrid(renderer, camera.getView({ 800, 600 })); // Assuming window size
}

void LevelEditor::DrawUI(sf::RenderWindow& window)
{
    if (!editorMode) return;

    sf::View originalView = window.getView();
    window.setView(window.getDefaultView());

    // Draw instruction text
    instructionText.setPosition(10, 10);
    window.draw(instructionText);

    // Draw tile selector
    tileSelector.setPosition(10, window.getSize().y - 100);
    window.draw(tileSelector);

    tileTypeText.setPosition(20, window.getSize().y - 90);
    window.draw(tileTypeText);

    window.setView(originalView);
}

void LevelEditor::SetSelectedTile(TileType type)
{
    selectedTileType = type;
}

sf::Vector2i LevelEditor::ScreenToGrid(const sf::Vector2i& screenPos, const sf::View& view)
{
    sf::Vector2f worldPos = view.getCenter() - view.getSize() / 2.0f;
    worldPos.x += (screenPos.x / 800.0f) * view.getSize().x;
    worldPos.y += (screenPos.y / 600.0f) * view.getSize().y;

    return sf::Vector2i(
        static_cast<int>(worldPos.x / cellSize),
        static_cast<int>(worldPos.y / cellSize)
    );
}

sf::Vector2f LevelEditor::GridToScreen(const sf::Vector2i& gridPos)
{
    return sf::Vector2f(
        gridPos.x * cellSize + cellSize / 2.0f,
        gridPos.y * cellSize + cellSize / 2.0f
    );
}

void LevelEditor::PlaceTile(const sf::Vector2i& gridPos)
{
    if (gridPos.x >= 0 && gridPos.y >= 0 &&
        gridPos.x < static_cast<int>(mapRef.grid.size()) &&
        gridPos.y < static_cast<int>(mapRef.grid[0].size())) {
        mapRef.grid[gridPos.x][gridPos.y] = static_cast<int>(selectedTileType);
    }
}

void LevelEditor::EraseTile(const sf::Vector2i& gridPos)
{
    if (gridPos.x >= 0 && gridPos.y >= 0 &&
        gridPos.x < static_cast<int>(mapRef.grid.size()) &&
        gridPos.y < static_cast<int>(mapRef.grid[0].size())) {
        mapRef.grid[gridPos.x][gridPos.y] = static_cast<int>(TileType::EMPTY);
    }
}

void LevelEditor::DrawGrid(Renderer& renderer, const sf::View& view)
{
    // This would require modifying the renderer to support line drawing
    // For now, we'll skip the grid visualization
    // You could implement this by drawing thin rectangles or using sf::Vertex arrays
}

void LevelEditor::SaveLevel(const std::string& filename)
{
    std::ofstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        std::cout << "Error: Could not save level to " << filename << std::endl;
        return;
    }

    // Save dimensions
    size_t width = mapRef.grid.size();
    size_t height = width > 0 ? mapRef.grid[0].size() : 0;

    file.write(reinterpret_cast<const char*>(&width), sizeof(width));
    file.write(reinterpret_cast<const char*>(&height), sizeof(height));

    // Save grid data
    for (size_t x = 0; x < width; ++x) {
        for (size_t y = 0; y < height; ++y) {
            int tileValue = mapRef.grid[x][y];
            file.write(reinterpret_cast<const char*>(&tileValue), sizeof(tileValue));
        }
    }

    file.close();
    std::cout << "Level saved to " << filename << std::endl;
}

void LevelEditor::LoadLevel(const std::string& filename)
{
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        std::cout << "Error: Could not load level from " << filename << std::endl;
        return;
    }

    // Load dimensions
    size_t width, height;
    file.read(reinterpret_cast<char*>(&width), sizeof(width));
    file.read(reinterpret_cast<char*>(&height), sizeof(height));

    // Resize grid
    mapRef.grid.clear();
    mapRef.grid.resize(width, std::vector<int>(height, 0));

    // Load grid data
    for (size_t x = 0; x < width; ++x) {
        for (size_t y = 0; y < height; ++y) {
            int tileValue;
            file.read(reinterpret_cast<char*>(&tileValue), sizeof(tileValue));
            mapRef.grid[x][y] = tileValue;
        }
    }

    file.close();
    std::cout << "Level loaded from " << filename << std::endl;
}

void LevelEditor::ClearLevel()
{
    for (auto& column : mapRef.grid) {
        for (auto& cell : column) {
            cell = static_cast<int>(TileType::EMPTY);
        }
    }
    std::cout << "Level cleared" << std::endl;
}

void LevelEditor::NewLevel(int width, int height)
{
    mapRef.grid.clear();
    mapRef.grid.resize(width, std::vector<int>(height, static_cast<int>(TileType::EMPTY)));
    std::cout << "New level created: " << width << "x" << height << std::endl;
}

std::string LevelEditor::TileTypeToString(TileType type)
{
    switch (type) {
    case TileType::EMPTY: return "Empty";
    case TileType::SOLID_BLOCK: return "Solid Block";
    case TileType::SPIKE: return "Spike";
    case TileType::DOOR: return "Door";
    default: return "Unknown";
    }
}