#include "engine.h"
#include "../draw/init.h"
#include "../draw/camera.h"
#include "controls.h"
#include "../../assets/Models/Robot.h"
#include "rockets.h"
#include <math.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

/* Game state variables can go here */
float robo_velocityX = 0.0f;
float robo_velocityY = 0.0f;
float robo_velocityZ = -30.0f;

float gravity = 30.0f; // units per second squared
float airSpeedCof = 8.0f;
float rocketSpeedCof = 60.0f;
float rocketJumpCof = 1.6f;

float accumulatedTime = 0.0f;

Rocket *ride_rocket = NULL;

int isInAir = 0;

void initEngine(void)
{
    robot = robot_create();
    robot_init(robot, 0.0f, 20.0f, 0.0f);

    rockets_init();
    rockets_spawn(0.0f, 10.0f, 0.0f, 0.0f, 0.0f, -30.0f);

    robo_velocityZ = -30.0f;
}

// Helper function to generate random float in range
static float randomFloat(float min, float max)
{
    float scale = rand() / (float)RAND_MAX; // 0.0 to 1.0
    return min + scale * (max - min);
}

static void move_robot_air(double deltaTime)
{
    robo_velocityX += controlState.moveX * (float)deltaTime * airSpeedCof;
    robo_velocityZ -= controlState.moveZ * (float)deltaTime * airSpeedCof;
    robo_velocityY -= gravity * (float)deltaTime;

    if (robot->core->y < 2.0f)
    {
        robo_velocityY = fabs(robo_velocityY) * 0.979f;
    }

    robot->core->x += robo_velocityX * (float)deltaTime;
    robot->core->y += robo_velocityY * (float)deltaTime;
    robot->core->z += robo_velocityZ * (float)deltaTime;
}

static void move_robot_riding(double deltaTime)
{
    robo_velocityX += controlState.moveX * (float)deltaTime * rocketSpeedCof;
    robo_velocityY -= controlState.moveZ * (float)deltaTime * rocketSpeedCof;

    robot->core->x += robo_velocityX * (float)deltaTime;
    robot->core->y += robo_velocityY * (float)deltaTime;
    robot->core->z += robo_velocityZ * (float)deltaTime;

    robot->core->rotY = -atan2f(robo_velocityX, -robo_velocityZ) * (180.0f / 3.14159f);
    robot->core->rotX = atan2f(robo_velocityY, -robo_velocityZ) * (180.0f / 3.14159f);
    robot->core->rotZ -= controlState.moveX * 2.0f;
    robot->core->rotZ = fmin(fmax(robot->core->rotZ,-30.0f), 30.0f);

    if (controlState.jump)
    {
        isInAir = 1;
        let_go_rocket(robot);
        rockets_remove(ride_rocket);
        robo_velocityX *= rocketJumpCof;
        robo_velocityY *= rocketJumpCof;
        robo_velocityZ *= rocketJumpCof;
        robo_velocityY += 10.0f;
        ride_rocket = NULL;
    }
}

static void update_all_rockets(double deltaTime)
{
    accumulatedTime += (float)deltaTime;
    if((int)round(accumulatedTime) % 2 == 0)
    {
        accumulatedTime = 1.2f;
        rockets_spawn( robot->core->x + randomFloat(-60, 60), randomFloat(0, 60), robot->core->z-randomFloat(60, 160), 0.0f, 0.0f, -30.0f);
    }
    rockets_update(deltaTime, robot->core->z + 30.0f);
}

void updateEngine(double deltaTime)
{
    float CrunchAngle = fminf(30.0f + controlState.moveZ * 60.0f, 60.0f); // Crunch more when moving up
    float SpinSpeed = 500.0f + (250.0f * controlState.moveZ);
    float RollAngle = controlState.moveX * 45.0f;
    robot_update(robot, deltaTime);
    update_all_rockets(deltaTime);

    if (!ride_rocket)
    {
        isInAir = 1;
        ride_rocket = rockets_findCollision(robot->core->x, robot->core->y, robot->core->z, 6.0f);
    }
    if(ride_rocket)
    {
        if(!ride_rocket->shell->parent){
            grab_rocket(robot, ride_rocket);
            robo_velocityZ = ride_rocket->rocket_velocityZ;
            robo_velocityX *= 0.0f;
            robo_velocityY = 0.0f;
        }
        isInAir = 0;
    }

    if (robot && robot->core && isInAir)
    {
        robot_inAirAnimation(robot, deltaTime, SpinSpeed, 0.3f, CrunchAngle, RollAngle);
        move_robot_air(deltaTime);
    }
    else
    {
        robot_riding_animation(robot, deltaTime);
        move_robot_riding(deltaTime);
    }
    camera_update(robot->core, robo_velocityX, robo_velocityZ, deltaTime);
}
