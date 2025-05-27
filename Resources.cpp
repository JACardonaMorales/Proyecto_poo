#include "stdafx.h"
#include "Resources.h"

std::unordered_map<std::string, std::unique_ptr<sf::Texture>> Resources::textures{};

sf::Texture* Resources::GetTexture(const std::string& name)
{
    auto it = textures.find(name);
    if (it != textures.end() && it->second) {
        return it->second.get();
    }
    return nullptr;
}

bool Resources::LoadTexture(const std::string& name, const std::string& filename)
{
    auto texture = std::make_unique<sf::Texture>();
    if (texture->loadFromFile(filename)) {
        textures[name] = std::move(texture);
        return true;
    }
    return false;
}

bool Resources::AddTexture(const std::string& name, const sf::Texture& texture)
{
    auto newTexture = std::make_unique<sf::Texture>();
    *newTexture = texture; // Copy the texture
    textures[name] = std::move(newTexture);
    return true;
}

void Resources::Clear()
{
    textures.clear();
}