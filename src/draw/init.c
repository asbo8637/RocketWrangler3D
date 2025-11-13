#include "init.h"
#include <stddef.h>
#include "../../assets/Models/Robot.h"
#include "texture.h"
#include "../game/rockets.h"

// Ground texture (0 means not available)
unsigned int groundTexture = 0u;
Robot *robot = NULL;

// Initialize all scene objects once at startup
void initScene(void)
{
    // Load ground texture (seamless, tiling)
    groundTexture = loadTexture2D("assets/Textures/close-up-mixture-clay-powder.jpg");
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
