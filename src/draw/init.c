#include "init.h"
#include <stddef.h>
#include "../../assets/Models/Robot.h"
#include "texture.h"
#include "../game/rockets.h"

// Background texture (0 means not available)
unsigned int backgroundTexture = 0u;
unsigned int sandTexture = 0u;
Robot *robot = NULL;

// Initialize all scene objects once at startup
void initScene(void)
{
    backgroundTexture = loadTexture2D("assets/Textures/istockphoto-510322864-612x612.jpg");
}

// Cleanup scene objects on shutdown
void cleanupScene(void)
{
    if (robot)
    {
        robot_destroy(robot);
        robot = NULL;
    }

    rockets_shutdown();

    // Cleanup other objects here
}
