#ifndef OBJECT_H
#define OBJECT_H

class Object {
public:
    float horizontal_speed;
    float vertical_speed;
    float x, y;

    Object(float x, float y, float horizontal_speed, float vertical_speed);
};

#endif