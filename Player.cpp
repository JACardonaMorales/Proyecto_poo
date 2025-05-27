#include "stdafx.h"
#include "Player.h"
#include "Resources.h"

Player::Player() {
    initVariables();
    initTexture();
    initSprite();
    initAnimations();
    initPhysics();
}

Player::~Player() {}

void Player::initVariables() {
    animState = IDLE;
    animationSwitch = false;
}

void Player::initTexture() {
    // Texture loading is handled in Game.cpp init
}

void Player::initSprite() {
    sf::Texture* texture = Resources::GetTexture("RaySprite");
    if (texture) {
        sprite.setTexture(*texture);
    }
    sprite.setScale(2.0f, 2.0f);
    currentFrame = sf::IntRect(0, 0, 32, 32);
    sprite.setTextureRect(currentFrame);
}

void Player::initAnimations() {
    animationTimer.restart();
}

void Player::initPhysics() {
    velocityMax = 5.0f;
    velocityMin = 1.0f;
    acceleration = 0.3f;
    drag = 0.85f;
    gravity = 0.5f;
    velocityMaxY = 15.0f;

    velocity.x = 0.0f;
    velocity.y = 0.0f;

    canJump = false;
    isOnGround = false;
    isOnPlatform = false;
    isOnLadder = false;
}

// Getters
const bool& Player::getAnimSwitch() { return animationSwitch; }
const sf::Vector2f Player::getPosition() const { return sprite.getPosition(); }
const sf::FloatRect Player::getGlobalBounds() const { return sprite.getGlobalBounds(); }
const sf::Vector2f& Player::getVelocity() const { return velocity; }

// Setters
void Player::setPosition(const float x, const float y) {
    sprite.setPosition(x, y);
}

void Player::resetVelocityY() { velocity.y = 0.0f; }
void Player::setVelocityY(const float velocity_y) { velocity.y = velocity_y; }
void Player::setVelocityX(const float velocity_x) { velocity.x = velocity_x; }

void Player::move(const float dir_x, const float dir_y) {
    velocity.x += dir_x * acceleration;
    velocity.y += dir_y * acceleration;
}

void Player::jump() {
    if (canJump) {
        velocity.y = -12.0f;
        canJump = false;
        isOnGround = false;
    }
}

void Player::updatePhysics() {
    // Gravity
    if (!isOnGround) {
        velocity.y += gravity;
    }

    // Limit velocity
    if (abs(velocity.x) > velocityMax) {
        velocity.x = velocityMax * ((velocity.x < 0.0f) ? -1.0f : 1.0f);
    }
    if (abs(velocity.y) > velocityMaxY) {
        velocity.y = velocityMaxY * ((velocity.y < 0.0f) ? -1.0f : 1.0f);
    }
}

void Player::updateMovement() {
    animState = IDLE;

    // Horizontal movement
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A) || sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
        move(-1.0f, 0.0f);
        animState = MOVING_LEFT;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D) || sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
        move(1.0f, 0.0f);
        animState = MOVING_RIGHT;
    }

    // Jumping
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {
        jump();
        animState = JUMPING;
    }

    // Apply drag
    velocity.x *= drag;

    // Apply physics
    updatePhysics();

    // Move sprite
    sprite.move(velocity);
}

void Player::updateAnimations() {
    if (animationTimer.getElapsedTime().asSeconds() >= 0.1f) {
        animationSwitch = !animationSwitch;
        animationTimer.restart();
    }
}

void Player::update() {
    updateMovement();
    updateAnimations();
}

void Player::render(sf::RenderTarget& target) {
    target.draw(sprite);
}