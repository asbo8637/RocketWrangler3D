#include "engine.h"
#include "../draw/init.h"
#include "../draw/camera.h"
#include "controls.h"
#include "../../assets/Models/Robot.h"
#include <math.h>

/* Game state variables can go here */
float robo_velocityX = 0.0f;
float robo_velocityY = 0.0f;
float robo_velocityZ = -30.0f;

float gravity = 70.0f; // units per second squared
float speedCof = 12.0f;

void initEngine(void)
{
    /* Initialize game state */
}

void move_robot(double deltaTime)
{
    robo_velocityX += controlState.moveX * (float)deltaTime * speedCof;
    robo_velocityZ -= controlState.moveZ * (float)deltaTime * speedCof;
    robo_velocityY -= gravity * (float)deltaTime;

    if(robot->core->y < 0.0f)
    {
        robo_velocityY = fabs(robo_velocityY);
    }

    robot->core->x += robo_velocityX * (float)deltaTime;
    robot->core->y += robo_velocityY * (float)deltaTime;
    robot->core->z += robo_velocityZ * (float)deltaTime;
}

void update_camera(double deltaTime)
{
    tpsTargetX = robot->core->x;
    tpsTargetY = robot->core->y + 5.0f;
    tpsTargetZ = robot->core->z;
}


void updateEngine(double deltaTime)
{
    /* Update game state based on deltaTime
       This will run every frame with deltaTime in seconds */

    /* Robot animation and update */
    float CrunchAngle = fminf(30.0f + controlState.moveZ * 60.0f, 65.0f); // Crunch more when moving up
    float SpinSpeed = 350.0f + (180.0f * controlState.moveZ);
    float RollAngle = controlState.moveX * 45.0f; 
    robot_inAirAnimation(robot, deltaTime, SpinSpeed, 0.3f, CrunchAngle, RollAngle);
    robot_update(robot, deltaTime);


    if (robot && robot->core)
    {   
        move_robot(deltaTime);
    }
    update_camera(deltaTime);
}
