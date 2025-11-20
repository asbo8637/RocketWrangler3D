#include "engine.h"
#include "../draw/init.h"
#include "../draw/camera.h"
#include "../draw/drawScene.h"
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

const float gravity = 75.0f; // units per second squared
const float startHeight = 50.0f;
const float airSpeedCof = 25.0f;
const float rocketSpeedCof = 110.0f;
const float rideSpeedCof = 130.0f;
const float rocketAngleCof = 30.0f;
const float rocketJumpCof = 1.4f;
const float rocketSpawnDistance = 300.0f;
const float deg2rad = 3.14159f / 180.0f;
int bounces = 0;
static int score = -1;

float accumulatedTime = 0.0f;

Rocket *ride_rocket = NULL;

int isInAir = 0;

// Helper function to generate random float in range
static float randomFloat(float min, float max)
{
    float scale = rand() / (float)RAND_MAX; // 0.0 to 1.0
    return min + scale * (max - min);
}


static void restart(){
    setSeed(); //From drawScene.c to reset ground seed
    if(robot)
        robot_destroy(robot);
    robot = robot_create();
    robot_init(robot, 0.0f, startHeight, 2.5f);
    robo_velocityX = 0.0f;
    robo_velocityY = 0.0f;
    robo_velocityZ = -rocketSpeedCof;
    score = -1;

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
        rockets_spawn(randomFloat(-90, 90), randomFloat(10, 90), robot->core->z-randomFloat(60, rocketSpawnDistance), 0.0f, 0.0f, -rocketSpeedCof);

}

void initEngine(void)
{
    restart();
}

static int bounce(){
    if (robot->core->y < 2.0f)
    {
        robo_velocityY = fabs(robo_velocityY) * 0.979f;
        robot->core->y = 2.0f;
        if(robo_velocityY > 100.0f)
            robo_velocityY = 100+robo_velocityY*0.3f;
        return 2;
    }
    else if (get_groundHeight(robot->core->x, robot->core->z)!= 0 && 
    robot->core->y < get_groundHeight(robot->core->x, robot->core->z) + 4.0f)
    {
        float old_robo_velocityY = robo_velocityY;
        robo_velocityY = (5.0f*fabs(robo_velocityX) + 5.25f*robo_velocityY)/7.5f;
        robo_velocityY = fmin(robo_velocityY, 60.0f);
        if(robot->core->x > 0.0f){
            robo_velocityX = (-robo_velocityX*5.25f + old_robo_velocityY*5.0f)/7.25f; 
            robot->core->x -= 2.0f;
        }
        else{
            printf("Bounce! Old: %f\n", robo_velocityX);
            robo_velocityX = (-robo_velocityX*5.25f - old_robo_velocityY*5.0f)/7.25f;
            robot->core->x += 2.0f;
            printf("Bounce! New VelX: %f\n", robo_velocityX);
        }
        robo_velocityX = fmin(fmax(robo_velocityX, -60.0f), 60.0f);
        return 1;  
    }
    return 0;
}

static void move_robot_air(double deltaTime)
{
    robo_velocityX += controlState.moveX * (float)deltaTime * airSpeedCof;
    robo_velocityY -= gravity * (float)deltaTime * powf(1.4f, controlState.moveZ);
    robo_velocityZ += controlState.moveZ * (float)deltaTime * airSpeedCof * 0.3f;

    if(bounce()==2)
    {
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

    robo_velocityX = -rideSpeedCof * dirX;
    robo_velocityY = rideSpeedCof * dirY;
    robo_velocityZ = -rideSpeedCof * dirZ;

    robot->core->x += robo_velocityX * (float)deltaTime;
    robot->core->y += robo_velocityY * (float)deltaTime;
    robot->core->z += robo_velocityZ * (float)deltaTime;

    if (controlState.jump || bounce())
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
        rockets_spawn(randomFloat(-90, 90), randomFloat(10, 90), robot->core->z-randomFloat(rocketSpawnDistance, rocketSpawnDistance+240), 0.0f, 0.0f, -rocketSpeedCof);
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
            score++;
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

int engine_getScore(void)
{
    return score;
}
