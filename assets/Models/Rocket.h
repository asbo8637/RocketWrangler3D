#ifndef ROCKET_H
#define ROCKET_H

#include "joint_class.h"

// Rocket "class" structure
typedef struct Rocket
{
    Joint *shell;

    // Animation state for in-air animation
    float rocket_velocityX;
    float rocket_velocityY;
    float rocket_velocityZ;
} Rocket;

// Create a new rocket instance
Rocket *rocket_create(void);
// Initialize rocket joints and hierarchy
void rocket_init(Rocket *rocket, float x, float y, float z, float vx, float vy, float vz);
// Update rocket animation
void rocket_update(Rocket *rocket, float deltaTime);
// Draw the rocket
void rocket_draw(const Rocket *rocket);
// Destroy rocket and release memory
void rocket_destroy(Rocket *rocket);
#endif // ROCKET_H
