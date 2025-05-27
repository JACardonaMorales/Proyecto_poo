#pragma once
#include <SFML/Graphics.hpp>

enum PLAYER_ANIMATION_STATES {
	IDLE = 0,
	MOVING_LEFT,
	MOVING_RIGHT,
	JUMPING,
	FALLING,
	CLIMBING
};

class Player
{
private:
	sf::Sprite sprite;
	sf::Texture textureSheet;
	sf::Clock animationTimer;

	//Animacion
	short animState;
	sf::IntRect currentFrame;
	bool animationSwitch;

	//Fisicas
	sf::Vector2f velocity;
	float velocityMax;
	float velocityMin;
	float acceleration;
	float drag;
	float gravity;
	float velocityMaxY;
	bool canJump;
	bool isOnGround;
	bool isOnPlatform;
	bool isOnLadder;

	//Core
	void initVariables();
	void initTexture();
	void initSprite();
	void initAnimations();
	void initPhysics();

public:
	Player();
	virtual ~Player();


	//Accessors
	const bool& getAnimSwitch();
	const sf::Vector2f getPosition() const;
	const sf::FloatRect getGlobalBounds() const;
	const sf::Vector2f& getVelocity() const;

	//Inline functions

	// Métodos adicionales necesarios para el sistema de físicas
	void setIsOnGround(bool onGround) { isOnGround = onGround; }
	void setCanJump(bool canJump) { this->canJump = canJump; }
	void setIsOnPlatform(bool onPlatform) { isOnPlatform = onPlatform; }
	

	// Getters adicionales
	bool getIsOnGround() const { return isOnGround; }
	bool getCanJump() const { return canJump; }
	bool getIsOnPlatform() const { return isOnPlatform; }
	

	// Método para establecer posición individual

	void setPosition(const sf::Vector2f& position) { sprite.setPosition(position); }

	//Modifiers
	void setPosition(const float x, const float y);
	void resetVelocityY();
	void setVelocityY(const float velocity_y);
	void setVelocityX(const float velocity_x);

	//Functions	
	void resetAnimationTimer();
	void move(const float dir_x, const float dir_y);
	void climb(const float dir_y);
	void jump();
	void updatePhysics();
	void updateMovement();
	void updateAnimations();
	void update();
	void render(sf::RenderTarget& target);
};