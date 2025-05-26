#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include <unordered_map>

class SpriteLoader
{
public:
    static SpriteLoader& getInstance();

    // Platform sprite management
    void LoadPlatformSprite(const std::string& filename);
    void CreateDefaultPlatformSprite();
    sf::Texture& GetPlatformTexture();

    // General sprite loading
    bool LoadSprite(const std::string& name, const std::string& filename);
    sf::Texture* GetTexture(const std::string& name);

    // Tileset management
    void LoadTileset(const std::string& filename, int tileWidth = 32, int tileHeight = 32);
    sf::IntRect GetTileRect(int tileX, int tileY) const;

    // Animation management
    struct AnimationData {
        std::string textureName;
        int frameCount;
        int frameWidth;
        int frameHeight;
        float frameTime;
        bool loop;

        AnimationData() : frameCount(1), frameWidth(32), frameHeight(32), frameTime(0.1f), loop(true) {}
    };

    void RegisterAnimation(const std::string& name, const AnimationData& data);
    sf::IntRect GetAnimationFrame(const std::string& animName, float currentTime) const;

private:
    SpriteLoader() = default;
    ~SpriteLoader() = default;
    SpriteLoader(const SpriteLoader&) = delete;
    SpriteLoader& operator=(const SpriteLoader&) = delete;

    std::unordered_map<std::string, sf::Texture> textures;
    std::unordered_map<std::string, AnimationData> animations;

    // Platform specific
    sf::Texture platformTexture;
    bool platformLoaded = false;

    // Tileset specific
    int tilesetTileWidth = 16;
    int tilesetTileHeight = 16;

    void CreatePlaceholderPlatform();
};