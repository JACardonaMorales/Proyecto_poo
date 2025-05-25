#include "stdafx.h"
#include "Player.h"

void Player::initVariables()
{
    this->animState = PLAYER_ANIMATION_STATES::IDLE;
    this->animationSwitch = true;

    this->canJump = true;
    this->isOnGround = false;
    this->isOnPlatform = false;
    this->isOnLadder = false;
}

void Player::initTexture()
{
    // Cargar textura del jugador
    if (!this->textureSheet.loadFromFile("assets/Ray/textures/RaySprite.png"))
    {
        std::cout << "ERROR::PLAYER::Could not load the player sheet!" << "\n";
    }
}

void Player::initSprite()
{
    this->sprite.setTexture(this->textureSheet);
    this->currentFrame = sf::IntRect(0, 0, 32, 32); // Ajusta según el tamaño real de tu sprite
    this->sprite.setTextureRect(this->currentFrame);
    this->sprite.setScale(2.f, 2.f);
}

void Player::initAnimations()
{
    this->animationTimer.restart();
}

void Player::initPhysics()
{
    this->velocityMax = 15.f;
    this->velocityMin = 1.f;
    this->acceleration = 2.f;
    this->drag = 0.85f;
    this->gravity = 0.8f;
    this->velocityMaxY = 15.f;
}

// Constructor y Destructor
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

// Accessors
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

const sf::Vector2f& Player::getVelocity() const
{
    return this->velocity;
}

// Modifiers
void Player::setPosition(const float x, const float y)
{
    this->sprite.setPosition(x, y);
}

void Player::resetVelocityY()
{
    this->velocity.y = 0.f;
}

void Player::setVelocityY(const float velocity_y)
{
    this->velocity.y = velocity_y;
}

void Player::setVelocityX(const float velocity_x)
{
    this->velocity.x = velocity_x;
}

// Functions
void Player::resetAnimationTimer()
{
    this->animationTimer.restart();
    this->animationSwitch = true;
}

void Player::move(const float dir_x, const float dir_y)
{
    // Aceleración
    this->velocity.x += dir_x * this->acceleration;

    // Si no estamos en escalera, caemos por gravedad
    if (!this->isOnLadder)
    {
        // Limita la velocidad máxima de caída
        if (this->velocity.y < this->velocityMaxY)
            this->velocity.y += this->gravity;
    }
    else
    {
        // Si estamos en escalera, controlamos la velocidad vertical manualmente
        this->velocity.y = dir_y * this->velocityMax;
    }

    // Limita la velocidad
    if (std::abs(this->velocity.x) > this->velocityMax)
    {
        this->velocity.x = this->velocityMax * ((this->velocity.x < 0.f) ? -1.f : 1.f);
    }
}

void Player::climb(const float dir_y)
{
    if (this->isOnLadder)
    {
        this->velocity.y = dir_y * this->velocityMax * 0.5f; // Velocidad de escalada más lenta
    }
}

void Player::jump()
{
    if (this->canJump && (this->isOnGround || this->isOnPlatform))
    {
        this->velocity.y = -16.f; // Ajusta la fuerza de salto según necesites
        this->canJump = false;
        this->isOnGround = false;
        this->isOnPlatform = false;
    }
}

void Player::updatePhysics()
{
    // Aplicar gravedad (si no está en escalera)
    if (!this->isOnLadder)
    {
        // Limita la velocidad máxima de caída
        if (this->velocity.y < this->velocityMaxY)
            this->velocity.y += this->gravity;
    }

    // Desaceleración
    this->velocity.x *= this->drag;

    // Si la velocidad es muy pequeña, la hacemos cero
    if (std::abs(this->velocity.x) < this->velocityMin)
        this->velocity.x = 0.f;

    // Movimiento
    this->sprite.move(this->velocity.x, this->velocity.y);
}

void Player::updateMovement()
{
    // Actualizar el estado de animación basado en la velocidad
    if (this->velocity.x < 0.f)
        this->animState = PLAYER_ANIMATION_STATES::MOVING_LEFT;
    else if (this->velocity.x > 0.f)
        this->animState = PLAYER_ANIMATION_STATES::MOVING_RIGHT;
    else
        this->animState = PLAYER_ANIMATION_STATES::IDLE;

    // Actualizar estado de salto/caída
    if (this->velocity.y < 0.f)
        this->animState = PLAYER_ANIMATION_STATES::JUMPING;
    else if (this->velocity.y > 0.f && !this->isOnGround && !this->isOnPlatform)
        this->animState = PLAYER_ANIMATION_STATES::FALLING;

    // Si está en escalera
    if (this->isOnLadder)
        this->animState = PLAYER_ANIMATION_STATES::CLIMBING;
}

void Player::updateAnimations()
{
    if (this->animationTimer.getElapsedTime().asSeconds() >= 0.1f) // Controla la velocidad de la animación
    {
        // Cambiar el frame actual basado en el estado de animación
        switch (this->animState)
        {
        case PLAYER_ANIMATION_STATES::IDLE:
            this->currentFrame.top = 0;
            this->currentFrame.left += 32; // Ajusta según el tamaño real de tu sprite

            if (this->currentFrame.left >= 128) // Ajusta según el número de frames de tu animación
                this->currentFrame.left = 0;

            break;
        case PLAYER_ANIMATION_STATES::MOVING_LEFT:
            this->currentFrame.top = 32; // Ajusta según la fila de tu sprite sheet
            this->currentFrame.left += 32;

            if (this->currentFrame.left >= 128)
                this->currentFrame.left = 0;

            // Voltear sprite hacia la izquierda
            this->sprite.setScale(-2.f, 2.f);
            break;
        case PLAYER_ANIMATION_STATES::MOVING_RIGHT:
            this->currentFrame.top = 32;
            this->currentFrame.left += 32;

            if (this->currentFrame.left >= 128)
                this->currentFrame.left = 0;

            // Mantener sprite mirando a la derecha
            this->sprite.setScale(2.f, 2.f);
            break;
        case PLAYER_ANIMATION_STATES::JUMPING:
            this->currentFrame.top = 64;
            this->currentFrame.left = 0;
            break;
        case PLAYER_ANIMATION_STATES::FALLING:
            this->currentFrame.top = 64;
            this->currentFrame.left = 32;
            break;
        case PLAYER_ANIMATION_STATES::CLIMBING:
            this->currentFrame.top = 96;
            this->currentFrame.left += 32;

            if (this->currentFrame.left >= 128)
                this->currentFrame.left = 0;

            break;
        default:
            break;
        }

        // Actualizar el temporizador y aplicar el nuevo frame
        this->animationTimer.restart();
        this->sprite.setTextureRect(this->currentFrame);
    }
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
}