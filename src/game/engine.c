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

float gravity = 40.0f; // units per second squared
float startHeight = 50.0f;
float airSpeedCof = 12.0f;
float rocketSpeedCof = 70.0f;
float rocketAngleCof = 50.0f;
float rocketJumpCof = 1.6f;
float deg2rad = 3.14159f / 180.0f;
int bounces = 0;

float accumulatedTime = 0.0f;

Rocket *ride_rocket = NULL;

int isInAir = 0;

// Helper function to generate random float in range
static float randomFloat(float min, float max)
{
    float scale = rand() / (float)RAND_MAX; // 0.0 to 1.0
    return min + scale * (max - min);
}


void restart(){
    if(robot)
        robot_destroy(robot);
    robot = robot_create();
    robot_init(robot, 0.0f, startHeight, 2.5f);
    robo_velocityX = 0.0f;
    robo_velocityY = 0.0f;

    robot->core->rotX = randomFloat(-180.0f,180.0f);
    robot->core->rotY = randomFloat(-180.0f,180.0f);
    robot->core->rotZ = randomFloat(-180.0f,180.0f);

    isInAir = 1;
    bounces = 0;
    ride_rocket = NULL;

    robot->animateStyle = 0;

    rockets_shutdown();

    // Spawn initial rocket
    rockets_spawn(0.0f, startHeight, 0.0f, 0.0f, 0.0f, -rocketSpeedCof);

    for(int i=0; i<15; i++)
        rockets_spawn(randomFloat(-90, 90), randomFloat(10, 90), robot->core->z-randomFloat(60, 220), 0.0f, 0.0f, -rocketSpeedCof);

}

void initEngine(void)
{
    restart();
}

static void move_robot_air(double deltaTime)
{
    robo_velocityX += controlState.moveX * (float)deltaTime * airSpeedCof;
    //multiply gravity based on moveZ
    robo_velocityY -= gravity * (float)deltaTime * powf(1.4f, controlState.moveZ);
    robo_velocityZ += controlState.moveZ * (float)deltaTime * airSpeedCof * 0.25f ;

    if (robot->core->y < 2.0f)
    {
        robo_velocityY = fabs(robo_velocityY) * 0.979f;
        robot->core->y = 2.0f;
        bounces++;
    }

    robot->core->x += robo_velocityX * (float)deltaTime;
    robot->core->y += robo_velocityY * (float)deltaTime;
    robot->core->z += robo_velocityZ * (float)deltaTime;
}



static void move_robot_riding(double deltaTime)
{
    robot->core->rotZ -= controlState.moveX * 3.0f * (float)deltaTime * rocketAngleCof;
    robot->core->rotZ = fmin(fmax(robot->core->rotZ,-70.0f), 70.0f);


    robot->core->rotY -= controlState.moveX * (float)deltaTime * rocketAngleCof;
    robot->core->rotX -= controlState.moveZ * (float)deltaTime * rocketAngleCof;

    float pitch = robot->core->rotX * deg2rad;   // rotX
    float yaw   = robot->core->rotY * deg2rad;   // rotY

    float dirX = cosf(pitch) * sinf(yaw);
    float dirY = sinf(pitch);
    float dirZ = cosf(pitch) * cosf(yaw);

    robo_velocityX = -rocketSpeedCof * dirX;
    robo_velocityY = rocketSpeedCof * dirY;
    robo_velocityZ = -rocketSpeedCof * dirZ;

    robot->core->x += robo_velocityX * (float)deltaTime;
    robot->core->y += robo_velocityY * (float)deltaTime;
    robot->core->z += robo_velocityZ * (float)deltaTime;

    if (controlState.jump)
    {
        isInAir = 1;
        let_go_rocket(robot);
        rockets_remove(ride_rocket);
        bounces=0;
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
        accumulatedTime = 1.6f;
        rockets_spawn( robot->core->x + randomFloat(-90, 90), randomFloat(10, 90), robot->core->z-randomFloat(220, 360), 0.0f, 0.0f, -rocketSpeedCof);
    }
    rockets_update(deltaTime, robot->core->z + 30.0f);
}

void updateEngine(double deltaTime)
{
    robot_update(robot, deltaTime);
    update_all_rockets(deltaTime);


    if (!ride_rocket)
    {
        isInAir = 1;
        ride_rocket = rockets_findCollision(robot->core->x, robot->core->y, robot->core->z, 2.0f);
    }
    if(ride_rocket)
    {
        if(!ride_rocket->shell->parent){
            grab_rocket(robot, ride_rocket);
            robo_velocityZ = ride_rocket->rocket_velocityZ-30.0f;
            robo_velocityX *= 0.0f;
            robo_velocityY = 0.0f;
            bounces = 0;
            robot->animateStyle = (int)roundl(randomFloat(2.0f, 4.0f)) % 2 + 1; //1 or 2
        }
        isInAir = 0;
    }
    if (robot && robot->core && isInAir)
    {
        robot->animateStyle = 0;
        move_robot_air(deltaTime);
    }
    else
    {
        move_robot_riding(deltaTime);
    }

    if (controlState.restart || bounces == 2){
        restart();
    }
    animate_robot(robot, (float)deltaTime);
    camera_update(robot->core, robo_velocityX, robo_velocityZ, robo_velocityY, deltaTime);
}

int engine_getBounceCount(void)
{
    return bounces;
}
