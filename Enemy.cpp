#include "Global.h"
#include "Enemy.h"

Enemy::Enemy(float x, float y)
    : x(x), y(y), dead(false), horizontal_speed(0.0f), vertical_speed(0.0f) {
}

bool Enemy::get_dead(bool deletion) {
    return dead;
}

sf::FloatRect Enemy::get_hit_box() const {
    return sf::FloatRect(x, y, 16, 16);
}