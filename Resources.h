#pragma once
#include <SFML/Graphics.hpp>
#include <unordered_map>
#include <memory>
#include <string>

class Resources
{
public:
    // Texture management
    static sf::Texture* GetTexture(const std::string& name);
    static bool LoadTexture(const std::string& name, const std::string& filename);
    static bool AddTexture(const std::string& name, const sf::Texture& texture);
    static void Clear();

private:
    // Use function-local static to avoid initialization order issues
    static std::unordered_map<std::string, std::unique_ptr<sf::Texture>>& GetTexturesMap();
};