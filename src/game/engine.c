#include "engine.h"
#include "../draw/init.h"
#include "../draw/camera.h"
#include "controls.h"
#include "../../assets/Models/Robot.h"
#include <math.h>

/* Game state variables can go here */

void initEngine(void)
{
    /* Initialize game state */
}

void updateEngine(double deltaTime)
{
    /* Update game state based on deltaTime
       This will run every frame with deltaTime in seconds */

    /* Robot animation and update */
    float CrunchAngle = fminf(30.0f + controlState.moveZ * 60.0f, 60.0f); // Crunch more when moving up
    float SpinSpeed = 300.0f * (1 + controlState.moveZ * 0.5f);
    float RollAngle = controlState.moveX * 45.0f; 
    robot_inAirAnimation(robot, deltaTime, SpinSpeed, 0.3f, CrunchAngle, RollAngle);
    robot_update(robot, deltaTime);

    // Update camera target to follow robot core
    // Comment out to control camera.
    if (robot && robot->core)
    {
        tpsTargetX = robot->core->x;
        tpsTargetY = robot->core->y;
        tpsTargetZ = robot->core->z;
    }
}
