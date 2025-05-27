#ifndef ENEMY_H
#define ENEMY_H

#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>

class MapManager;
class Ray;

class Enemy : public std::enable_shared_from_this<Enemy> {
protected:
    bool dead;
    float horizontal_speed;
    float vertical_speed;
    float x, y;

public:
    Enemy(float x, float y);
    virtual ~Enemy() = default;

    virtual bool get_dead(bool deletion = false);
    virtual void die(int death_type) = 0;
    virtual void draw(float view_x, sf::RenderWindow& window) = 0;
    virtual void update(float view_x, std::vector<std::shared_ptr<Enemy>>& enemies,
        MapManager& map_manager, Ray& ray) = 0;
    virtual sf::FloatRect get_hit_box() const;

    float get_x() const { return x; }
    float get_y() const { return y; }
};

#endif