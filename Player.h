#pragma once

enum PLAYER_ANIMATION_STATES { IDLE = 0, MOVING_LEFT, MOVING_RIGHT, JUMPING, FALLING, CLIMBING };

class Player
{
private:
	sf::Sprite sprite;
	sf::Texture textureSheet;
	sf::Clock animationTimer;

	//Animation
	short animState;
	sf::IntRect currentFrame;
	bool animationSwitch;

	//Physics
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
	inline const bool& getCanJump() const { return this->canJump; }
	inline void setCanJump(const bool canJump) { this->canJump = canJump; }
	inline const bool& getIsOnGround() const { return this->isOnGround; }
	inline void setIsOnGround(const bool isOnGround) { this->isOnGround = isOnGround; }
	inline const bool& getIsOnPlatform() const { return this->isOnPlatform; }
	inline void setIsOnPlatform(const bool isOnPlatform) { this->isOnPlatform = isOnPlatform; }
	inline const bool& getIsOnLadder() const { return this->isOnLadder; }
	inline void setIsOnLadder(const bool isOnLadder) { this->isOnLadder = isOnLadder; }

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