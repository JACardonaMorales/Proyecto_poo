#include "stdafx.h"
#include "Resources.h"

// Solution 1: Use function-local static (Meyer's Singleton pattern)
std::unordered_map<std::string, std::unique_ptr<sf::Texture>>& Resources::GetTexturesMap()
{
    static std::unordered_map<std::string, std::unique_ptr<sf::Texture>> textures;
    return textures;
}

sf::Texture* Resources::GetTexture(const std::string& name)
{
    auto& texturesMap = GetTexturesMap();
    auto it = texturesMap.find(name);
    if (it != texturesMap.end() && it->second) {
        return it->second.get();
    }
    return nullptr;
}

bool Resources::LoadTexture(const std::string& name, const std::string& filename)
{
    auto texture = std::make_unique<sf::Texture>();
    if (texture->loadFromFile(filename)) {
        GetTexturesMap()[name] = std::move(texture);
        return true;
    }
    return false;
}

bool Resources::AddTexture(const std::string& name, const sf::Texture& texture)
{
    auto newTexture = std::make_unique<sf::Texture>();
    *newTexture = texture; 
    GetTexturesMap()[name] = std::move(newTexture);
    return true;
}

void Resources::Clear()
{
    GetTexturesMap().clear();
}