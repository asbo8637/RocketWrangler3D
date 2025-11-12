#include "init.h"
#include <stddef.h>
#include "../../assets/Models/Robot.h"
#include "texture.h"

// Global scene objects
Robot *robot = NULL;
// Ground texture (0 means not available)
unsigned int groundTexture = 0u;

// Initialize all scene objects once at startup
void initScene(void)
{
    // Create and initialize robot
    robot = robot_create();
    robot_init(robot, 0.0f, 20.0f, -5.0f);

    // Load ground texture (seamless, tiling)
    groundTexture = loadTexture2D("assets/Textures/close-up-mixture-clay-powder.jpg");

    // Add more objects here as needed:
    // arm = bj_create(...);
    // enemy = enemy_create(...);
}

// Cleanup scene objects on shutdown
void cleanupScene(void)
{
    if (robot)
    {
        robot_destroy(robot);
        robot = NULL;
    }

    // Cleanup other objects here
}
