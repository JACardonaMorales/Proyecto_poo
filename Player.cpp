#include "stdafx.h"
#include "Player.h"

void Player::initVariables()
{
	this->animState = PLAYER_ANIMATION_STATES::IDLE;
}

void Player::initTexture()
{
	if (!this->textureSheet.loadFromFile("assets/Ray/textures/RaySprite.png"))
	{
		std::cout << "ERROR::PLAYER::Las texturas no han sido encontradas" <<  std::endl;
	}
}

void Player::initSprite()
{
	this->sprite.setTexture(this->textureSheet);
	this->currentFrame = sf::IntRect(0, 0, 223 / 3, 298 / 2); 

	this->sprite.setTextureRect(this->currentFrame);
	this->sprite.setScale(3.f, 3.f);
}

void Player::initAnimations()
{
	this->animationTimer.restart();
	this->animationSwitch = true;
	this->currentFrame.left = 0.f;
}

void Player::initPhysics()
{
	this->velocityMax = 22.f;
	this->velocityMin = 2.0f;
	this->acceleration = 3.0f;
	this->drag = 0.80f;
	this->gravity = 3.f;
	this->velocityMaxY = 30.f;
	this->canJump = false;
}

Player::Player()
{
	this->initVariables();
	this->initTexture();
	this->initSprite();
	this->initAnimations();
	this->initPhysics();
}

Player::~Player()
{

}

const bool& Player::getAnimSwitch()
{
	bool anim_switch = this->animationSwitch;

	if (this->animationSwitch)
		this->animationSwitch = false;

	return anim_switch;
}

const sf::Vector2f Player::getPosition() const
{
	return this->sprite.getPosition();
}

const sf::FloatRect Player::getGlobalBounds() const
{
	return this->sprite.getGlobalBounds();
}

void Player::setPosition(const float x, const float y)
{
	this->sprite.setPosition(x, y);
}

void Player::resetVelocityY()
{
	this->velocity.y = 0.0;
}

void Player::resetAnimationTimer()
{
	this->animationTimer.restart();
	this->animationSwitch = true;
}

void Player::move(const float dir_x, const float dir_y)
{
	//Acceleration
	this->velocity.x += dir_x * this->acceleration;

	//Limit velocity
	if (std::abs(this->velocity.x) > this->velocityMax)
	{
		this->velocity.x = this->velocityMax * ((this->velocity.x < 0.f) ? -1.f : 1.f);
	}
}

void Player::jump()
{
	this->velocity.y = -70.f;
	this->canJump = false;
}

void Player::updatePhysics()
{
	//Gravity
	this->velocity.y += 1.0 * this->gravity;


	//Deceleration
	this->velocity *= this->drag;

	//Limit deceleration
	if (std::abs(this->velocity.x) < this->velocityMin)
		this->velocity.x = 0.0;
	if (std::abs(this->velocity.y) < this->velocityMin)
		this->velocity.y = 0.0;

	if (std::abs(this->velocity.x) <= 1.f)
		this->velocity.x = 0.0;

	this->sprite.move(this->velocity);
}

void Player::updateMovement()
{
	if (this->velocity.x > 0.0)
		this->animState = PLAYER_ANIMATION_STATES::MOVING_RIGHT;
	else if (this->velocity.x < 0.0)
		this->animState = PLAYER_ANIMATION_STATES::MOVING_LEFT;
	else
		this->animState = PLAYER_ANIMATION_STATES::IDLE;
}

void Player::updateAnimations()
{
	float speedPercent = (abs(this->velocity.x) / this->velocityMax);
	const float frameWidth = 223.f / 3;  // Ancho del sprite individual
	const float frameHeight = 298.f / 2;

	if (this->animState == PLAYER_ANIMATION_STATES::IDLE)
	{
		this->currentFrame.left = 0.f;
		this->currentFrame.top = 0.f;
		this->currentFrame.width = frameWidth;
		this->currentFrame.height = frameHeight;
		this->sprite.setTextureRect(this->currentFrame);
	}
	else if (this->animState == PLAYER_ANIMATION_STATES::MOVING_RIGHT)
	{
		if (this->animationTimer.getElapsedTime().asMilliseconds() >= 90.f / speedPercent || this->getAnimSwitch())
		{
			// Caminar: Primera columna, primera fila
   // De acuerdo a la imagen, parece que hay 3 frames en la primera columna
			this->currentFrameIndex = (this->currentFrameIndex + 1) % 3;

			// Calculamos la posición del frame actual en el spritesheet
			this->currentFrame.left = this->currentFrameIndex * frameWidth;
			this->currentFrame.top = 0.f; // Primera fila
			this->currentFrame.width = frameWidth;
			this->currentFrame.height = frameHeight;

			this->animationTimer.restart();
			this->sprite.setTextureRect(this->currentFrame);
		}

		this->sprite.setScale(3.f, 3.f);
		this->sprite.setOrigin(0.f, 0.f);
	}
	else if (this->animState == PLAYER_ANIMATION_STATES::MOVING_LEFT)
	{
		if (this->animationTimer.getElapsedTime().asMilliseconds() >= 90.f / speedPercent || this->getAnimSwitch())
		{
			// Mismos frames que para moverse a la derecha
			this->currentFrameIndex = (this->currentFrameIndex + 1) % 3;

			this->currentFrame.left = this->currentFrameIndex * frameWidth;
			this->currentFrame.top = 0.f; // Primera fila
			this->currentFrame.width = frameWidth;
			this->currentFrame.height = frameHeight;

			this->animationTimer.restart();
			this->sprite.setTextureRect(this->currentFrame);
		}

		this->sprite.setScale(-3.f, 3.f);
		this->sprite.setOrigin(this->sprite.getGlobalBounds().width / 3.f, 0.f);
	}
	else
		this->animationTimer.restart();
}

void Player::update()
{
	this->updateMovement();
	this->updateAnimations();
	this->updatePhysics();
}

void Player::render(sf::RenderTarget& target)
{
	target.draw(this->sprite);

	sf::CircleShape circ;
	circ.setFillColor(sf::Color::Red);
	circ.setRadius(2.f);
	circ.setPosition(this->sprite.getPosition());

	target.draw(circ);
}