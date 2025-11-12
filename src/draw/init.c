#include "init.h"
#include <stddef.h>
#include "../../assets/Models/Robot.h"

// Global scene objects
Robot *robot = NULL;

// Initialize all scene objects once at startup
void initScene(void)
{
    // Create and initialize robot
    robot = robot_create();
    robot_init(robot, 0.0f, 2.0f, -5.0f);

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
