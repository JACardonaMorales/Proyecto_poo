#include "stdafx.h"
#include "SpriteLoader.h"
#include "Resources.h"

SpriteLoader& SpriteLoader::getInstance()
{
    static SpriteLoader instance;
    return instance;
}

void SpriteLoader::LoadPlatformSprite(const std::string& filename)
{
    if (platformTexture.loadFromFile(filename)) {
        platformLoaded = true;
        // Also add to main resources for compatibility
        Resources::textures["platform"] = platformTexture;
        std::cout << "Platform sprite loaded successfully from: " << filename << std::endl;
    }
    else {
        std::cout << "Failed to load platform sprite from: " << filename << std::endl;
        std::cout << "Creating default platform sprite..." << std::endl;
        CreateDefaultPlatformSprite();
    }
}

void SpriteLoader::CreateDefaultPlatformSprite()
{
    sf::Image platformImage;

    // Create a more detailed wooden platform
    int width = 64;
    int height = 16;
    platformImage.create(width, height);

    // Base wood color
    sf::Color woodBase(139, 69, 19);
    sf::Color woodLight(160, 82, 45);
    sf::Color woodDark(101, 67, 33);
    sf::Color woodHighlight(205, 133, 63);

    // Fill with base color
    for (int x = 0; x < width; ++x) {
        for (int y = 0; y < height; ++y) {
            platformImage.setPixel(x, y, woodBase);
        }
    }

    // Add wood grain pattern
    for (int x = 0; x < width; x += 8) {
        for (int y = 0; y < height; ++y) {
            if (x < width) {
                platformImage.setPixel(x, y, woodDark);
                if (x + 1 < width) platformImage.setPixel(x + 1, y, woodLight);
            }
        }
    }

    // Add top highlight
    for (int x = 0; x < width; ++x) {
        platformImage.setPixel(x, 0, woodHighlight);
        if (height > 1) platformImage.setPixel(x, 1, woodLight);
    }

    // Add bottom shadow
    for (int x = 0; x < width; ++x) {
        platformImage.setPixel(x, height - 1, woodDark);
        if (height > 2) platformImage.setPixel(x, height - 2, woodDark);
    }

    // Add some nail details
    for (int x = 8; x < width; x += 16) {
        if (x < width) {
            platformImage.setPixel(x, 3, sf::Color(64, 64, 64)); // Dark nail
            platformImage.setPixel(x, height - 4, sf::Color(64, 64, 64));
        }
    }

    platformTexture.loadFromImage(platformImage);
    platformLoaded = true;
    Resources::textures["platform"] = platformTexture;

    std::cout << "Default platform sprite created successfully!" << std::endl;
}

sf::Texture& SpriteLoader::GetPlatformTexture()
{
    if (!platformLoaded) {
        CreateDefaultPlatformSprite();
    }
    return platformTexture;
}

bool SpriteLoader::LoadSprite(const std::string& name, const std::string& filename)
{
    sf::Texture texture;
    if (texture.loadFromFile(filename)) {
        textures[name] = texture;
        Resources::textures[name] = texture; // Also add to main resources
        std::cout << "Sprite '" << name << "' loaded from: " << filename << std::endl;
        return true;
    }
    else {
        std::cout << "Failed to load sprite '" << name << "' from: " << filename << std::endl;
        return false;
    }
}

sf::Texture* SpriteLoader::GetTexture(const std::string& name)
{
    auto it = textures.find(name);
    if (it != textures.end()) {
        return &it->second;
    }
    return nullptr;
}

void SpriteLoader::LoadTileset(const std::string& filename, int tileWidth, int tileHeight)
{
    tilesetTileWidth = tileWidth;
    tilesetTileHeight = tileHeight;

    if (LoadSprite("tileset", filename)) {
        std::cout << "Tileset loaded with tile size: " << tileWidth << "x" << tileHeight << std::endl;
    }
}

sf::IntRect SpriteLoader::GetTileRect(int tileX, int tileY) const
{
    return sf::IntRect(
        tileX * tilesetTileWidth,
        tileY * tilesetTileHeight,
        tilesetTileWidth,
        tilesetTileHeight
    );
}

void SpriteLoader::RegisterAnimation(const std::string& name, const AnimationData& data)
{
    animations[name] = data;
    std::cout << "Animation '" << name << "' registered with " << data.frameCount << " frames" << std::endl;
}

sf::IntRect SpriteLoader::GetAnimationFrame(const std::string& animName, float currentTime) const
{
    auto it = animations.find(animName);
    if (it != animations.end()) {
        const AnimationData& anim = it->second;

        float totalAnimTime = anim.frameCount * anim.frameTime;
        float normalizedTime = currentTime;

        if (anim.loop) {
            normalizedTime = fmod(currentTime, totalAnimTime);
        }
        else {
            normalizedTime = std::min(currentTime, totalAnimTime);
        }

        int currentFrame = (int)(normalizedTime / anim.frameTime);
        currentFrame = std::min(currentFrame, anim.frameCount - 1);

        return sf::IntRect(
            currentFrame * anim.frameWidth,
            0,
            anim.frameWidth,
            anim.frameHeight
        );
    }

    return sf::IntRect(0, 0, 32, 32); // Default frame
}