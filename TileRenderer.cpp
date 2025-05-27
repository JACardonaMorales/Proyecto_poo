#include "stdafx.h"
#include "TileRenderer.h"

void TileRenderer::InitializeTileInfos()
{
	tileInfos.clear();
	// Copia exacta del código de LevelEditor::InitializeTileInfos()
	tileInfos.push_back(TileInfo(TileType::EMPTY, false, false, sf::Vector2i(0, 0), ""));
	tileInfos.push_back(TileInfo(TileType::WALL, true, false, sf::Vector2i(0, 0), "wall"));
	tileInfos.push_back(TileInfo(TileType::PLATFORM, true, false, sf::Vector2i(0, 0), "platform"));
	tileInfos.push_back(TileInfo(TileType::SPIKES, true, false, sf::Vector2i(0, 0), "spikes"));
	tileInfos.push_back(TileInfo(TileType::TORCH, false, true, sf::Vector2i(0, 0), "torch"));
	tileInfos.push_back(TileInfo(TileType::DOOR, true, false, sf::Vector2i(0, 0), "door"));

	// Tileset blocks
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

void TileRenderer::RenderTile(sf::RenderWindow& window, TileType type, const sf::Vector2f& position, float animTime)
{
	if (type == TileType::TORCH) {
		RenderTorch(window, position, animTime);
		return;
	}

	for (const auto& tileInfo : tileInfos) {
		if (tileInfo.type == type && !tileInfo.textureName.empty()) {
			sf::Texture* texture = Resources::GetTexture(tileInfo.textureName);
			if (texture) {
				sf::Sprite sprite;
				sprite.setTexture(*texture);

				if (tileInfo.textureName == "tileset") {
					sf::IntRect rect = GetTilesetRect(type);
					sprite.setTextureRect(rect);
				}

				sprite.setOrigin(sprite.getLocalBounds().width / 2.0f, sprite.getLocalBounds().height / 2.0f);
				sprite.setPosition(position);

				// Scale to fit tile size - CRUCIAL PARA CONSISTENCIA
				float scaleX = tileSize / sprite.getLocalBounds().width;
				float scaleY = tileSize / sprite.getLocalBounds().height;
				sprite.setScale(scaleX, scaleY);

				window.draw(sprite);
			}
			break;
		}
	}
}

void TileRenderer::RenderTorch(sf::RenderWindow& window, const sf::Vector2f& position, float animTime)
{
	sf::Texture* texture = Resources::GetTexture("torch");
	if (texture) {
		sf::Sprite sprite;
		sprite.setTexture(*texture);

		// Calculate animation frame (assuming 8 frames horizontally)
		int frameWidth = texture->getSize().x / 8;
		int frameHeight = texture->getSize().y;
		int currentFrame = (int)(animTime * 8.0f) % 8;

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

sf::IntRect TileRenderer::GetTilesetRect(TileType type) const
{
	for (const auto& tileInfo : tileInfos) {
		if (tileInfo.type == type) {
			int tileWidth = 32;
			int tileHeight = 32;
			return sf::IntRect(
				tileInfo.textureCoord.x * tileWidth,
				tileInfo.textureCoord.y * tileHeight,
				tileWidth,
				tileHeight
			);
		}
	}
	return sf::IntRect(0, 0, 32, 32);
}

sf::Vector2f TileRenderer::GetWorldPosition(int gridX, int gridY, int gridWidth, int gridHeight) const
{
	float x = gridX * tileSize - (gridWidth * tileSize) / 2.0f + tileSize / 2.0f;
	float y = gridY * tileSize - (gridHeight * tileSize) / 2.0f + tileSize / 2.0f;
	return sf::Vector2f(x, y);
}
