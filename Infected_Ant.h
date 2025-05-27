#pragma once


#include "Global.h"
#include "Enemy.h"
#include "Animation.h"
#include <SFML/Graphics.hpp>

class Infected_Ant : public Enemy {
private:
    bool no_collision_dying;
    bool attacks;
    unsigned short death_timer;
    sf::Sprite sprite;
    sf::Texture texture;
    Animation walk_animation;

public:
    Infected_Ant(bool attacks, float x, float y);
    bool get_dead(bool deletion = false) override;
    void die(int death_type) override;
    void draw(float view_x, sf::RenderWindow& window) override;
    void update(float view_x, std::vector<std::shared_ptr<Enemy>>& enemies,
        MapManager& map_manager, Ray& ray) override;
};
