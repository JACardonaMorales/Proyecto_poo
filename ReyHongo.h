#ifndef REYHONGO_H
#define REYHONGO_H

#include "Enemy.h"
#include "Animation.h"
#include <SFML/Graphics.hpp>

class ReyHongo : public Enemy {
private:
    bool check_collision;
    bool flipped;
    bool no_collision_dying;
    bool attacks;
    unsigned char state;
    unsigned short get_out_timer;
    sf::Sprite sprite;
    sf::Texture texture;
    Animation get_out_animation;
    Animation walk_animation;

public:
    ReyHongo(bool attacks, float x, float y);
    bool get_dead(bool deletion = false) override;
    void die(int death_type) override;
    void draw(float view_x, sf::RenderWindow& window) override;
    void update(float view_x, std::vector<std::shared_ptr<Enemy>>& enemies,
        MapManager& map_manager, Ray& ray) override;
};

#endif