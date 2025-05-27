#pragma once
#include <SFML/Graphics.hpp>
#include <unordered_map>
#include <string>
#include <memory>

class Resources
{
public:
    // Usar punteros únicos para evitar problemas de copia de texturas
    static std::unordered_map<std::string, std::unique_ptr<sf::Texture>> textures;

    // Métodos helper para manejo seguro
    static sf::Texture* GetTexture(const std::string& name);
    static bool LoadTexture(const std::string& name, const std::string& filename);
    static bool AddTexture(const std::string& name, const sf::Texture& texture);
    static void Clear();
};