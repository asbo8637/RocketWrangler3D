#include "Robot.h"
#include "../Shapes/master.h"
#ifdef _WIN32
#include <windows.h>
#include <GL/glut.h>
#elif defined(__APPLE__)
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
#include "./Robot_positions/poses.h"
#include <stdlib.h>
#include <math.h>
#include "Rocket.h"

// Create a new rocket instance
Rocket *rocket_create(void)
{
    Rocket *rocket = (Rocket *)malloc(sizeof(Rocket));
    if (!rocket)
        return NULL;

    rocket->shell = NULL;
    rocket->rocket_velocityX = 0.0f;
    rocket->rocket_velocityY = 0.0f;
    rocket->rocket_velocityZ = 0.0f;

    return rocket;
}

void rocket_init(Rocket *rocket, float x, float y, float z, float vx, float vy, float vz)
{
    if (!rocket)
        return;

    // Create joints
    rocket->shell = joint_create();

    // Position shell joint in world (root of entire rocket)
    rocket->shell->x = x;
    rocket->shell->y = y;
    rocket->shell->z = z;
    
    rocket->rocket_velocityX = vx;
    rocket->rocket_velocityY = vy;
    rocket->rocket_velocityZ = vz;

    rocket->shell->rotX = 90.0f;
}

// Update rocket animation
void rocket_update(Rocket *rocket, float deltaTime)
{
    if (!rocket)
        return;
    // Update rocket position based on velocity
    rocket->shell->x += rocket->rocket_velocityX * deltaTime;
    rocket->shell->y += rocket->rocket_velocityY * deltaTime;
    rocket->shell->z += rocket->rocket_velocityZ * deltaTime;
}

// Draw the rocket
void rocket_draw(const Rocket *rocket)
{
    if (!rocket)
        return;

    float rocketColor[4] = {0.9f, 0.1f, 0.1f, 1.0f};
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, rocketColor);

    // Draw rocket shell
    glPushMatrix();
    joint_applyTransform(rocket->shell);
    drawCylinder(0.8f, 6.0f, 16);
    glPopMatrix();
}

void rocket_destroy(Rocket *rocket)
{
    if (!rocket)
        return;

    if (rocket->shell)
    {
        joint_free(rocket->shell);
        rocket->shell = NULL;
    }

    free(rocket);
}
