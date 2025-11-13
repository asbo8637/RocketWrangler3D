#include "engine.h"
#include "../draw/init.h"
#include "../draw/camera.h"
#include "controls.h"
#include "../../assets/Models/Robot.h"
#include "rockets.h"
#include <math.h>
#include <stddef.h>
#include <stdio.h>

/* Game state variables can go here */
float robo_velocityX = 0.0f;
float robo_velocityY = 0.0f;
float robo_velocityZ = -30.0f;

float gravity = 30.0f; // units per second squared
float speedCof = 8.0f;

int isInAir = 0;

void initEngine(void)
{
    robot = robot_create();
    robot_init(robot, 0.0f, 20.0f, 0.0f);

    rockets_init();
    rockets_spawn(0.0f, 10.0f, 0.0f);

    grab_rocket(robot, rockets_get(0));
}

static void move_robot_air(double deltaTime)
{
    robo_velocityX += controlState.moveX * (float)deltaTime * speedCof;
    robo_velocityZ -= controlState.moveZ * (float)deltaTime * speedCof;
    robo_velocityY -= gravity * (float)deltaTime;

    if(robot->core->y < 2.0f)
    {
        robo_velocityY = fabs(robo_velocityY)*0.979f;
    }

    robot->core->x += robo_velocityX * (float)deltaTime;
    robot->core->y += robo_velocityY * (float)deltaTime;
    robot->core->z += robo_velocityZ * (float)deltaTime;
}

static void update_camera(double deltaTime)
{
    tpsTargetX = robot->core->x;
    tpsTargetY = 10 + 0.5f * robot->core->y;
    tpsTargetZ = robot->core->z+10.0f;
    // tpsDist = fmin(fmax(-30.0f + 1.2f * fabs(robo_velocityZ), 10.0f), 22.0f);
    // tpsYaw = 1.57f - robo_velocityX * 0.01f;
}

static void update_all_rockets(double deltaTime)
{
    rockets_update(deltaTime);
}


void updateEngine(double deltaTime)
{
    float CrunchAngle = fminf(30.0f + controlState.moveZ * 60.0f, 60.0f); // Crunch more when moving up
    float SpinSpeed = 500.0f + (250.0f * controlState.moveZ);
    float RollAngle = controlState.moveX * 45.0f; 
    robot_update(robot, deltaTime);
    update_all_rockets(deltaTime);

    if (robot && robot->core && isInAir)
    {   
        robot_inAirAnimation(robot, deltaTime, SpinSpeed, 0.3f, CrunchAngle, RollAngle);
        move_robot_air(deltaTime);
    }
    else
    {
        robot_riding_animation(robot, deltaTime);
    }
    update_camera(deltaTime);
}
