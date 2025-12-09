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
#include "../game/particles.h"
#include <time.h>
#ifdef _WIN32
#include <windows.h>
#endif

/* Game state variables can go here */
float robo_velocityX = 0.0f;
float robo_velocityY = 0.0f;
float robo_velocityZ = -30.0f;

const float gravity = 75.0f; // units per second squared
const float gravityControlMult = 1.6f;
const float startHeight = 100.0f;
const float airSpeedCof = 55.0f;
const float baseRocketSpeedCof = 160.0f;
float rocketSpeedCof = 160.0f;
const float rocketAngleCof = 30.0f;
const float rocketJumpCof = 1.45f;
const float rocketSpawnDistance = 700.0f;
const float deg2rad = 3.14159f / 180.0f;
int bounces = 0;
float rocketRideTimer = 0.0f;
static int score = -1;
static const int MAX_PARTICLES = 800;

float accumulatedTime = 0.0f;

Rocket *ride_rocket = NULL;

int isInAir = 0;

// Helper function to generate random float in range
static float randomFloat(float min, float max)
{
    float scale = rand() / (float)RAND_MAX; // 0.0 to 1.0
    return min + scale * (max - min);
}

// Short pause when the game is restarted. This is here to ensure that the restart key press doesn't immediately retrigger a restart.
static void shortPause(void)
{
#ifdef _WIN32
    Sleep(30);
#else
    struct timespec ts;
    ts.tv_sec = 0;
    ts.tv_nsec = 30 * 1000000L; // 200 ms
    nanosleep(&ts, NULL);
#endif
}

//Restart the scene to start the game again. 
static void restart(){
    controlState.restart = 0.5f;
    shortPause();
    setSeed(); //From drawScene.c to reset ground seed
    if(robot)
        robot_destroy(robot);
    particles_shutdown();
    particles_init(MAX_PARTICLES);
    robot = robot_create();
    robot_init(robot, 0.0f, startHeight, 2.5f);
    robo_velocityX = 0.0f;
    robo_velocityY = 0.0f;
    robo_velocityZ = 0.0f;
    score = -1;
    rocketRideTimer = 0.0f;

    rocketSpeedCof = baseRocketSpeedCof;

    robot->core->rotX = 0.0f;
    robot->core->rotY = 0.0f;
    robot->core->rotZ = 0.0f;
    robot->core->z= 5.0f;

    isInAir = 1;
    bounces = 0;
    ride_rocket = NULL;

    robot->animateStyle = 0;

    rockets_shutdown();

    // Spawn initial rocket
    rockets_spawn(0.0f, startHeight, 0.0f, 0.0f, 0.0f, 0.0f);

    for(int i=0; i<20; i++)
        rockets_spawn(randomFloat(-80, 80), randomFloat(30, 200), robot->core->z-randomFloat(20, rocketSpawnDistance), 0.0f, 0.0f, -rocketSpeedCof);
    controlState.restart = 0.0f;
}

void initEngine(void)
{
    restart();
}

//Some complicated bounce logic to handle ground and wall bounces. Returns 2 for ground bounce, 1 for wall bounce, 0 for no bounce
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
            robo_velocityX = (-robo_velocityX*5.25f + old_robo_velocityY*5.0f)/3.25f; 
            robot->core->x -= 2.0f;
        }
        else{
            robo_velocityX = (-robo_velocityX*5.25f - old_robo_velocityY*5.0f)/3.25f;
            robot->core->x += 2.0f;
        }
        robo_velocityX = fmin(fmax(robo_velocityX, -30.0f), 30.0f);
        return 1;  
    }
    return 0;
}

// Movement when in air. Simple physics with gravity and user control
static void move_robot_air(double deltaTime)
{
    robo_velocityX += controlState.moveX * (float)deltaTime * airSpeedCof * 1.1f;
    robo_velocityX = fmaxf(robo_velocityX, -120.0f);
    robo_velocityX = fminf(robo_velocityX, 120.0f);
    robo_velocityY -= gravity * (float)deltaTime * powf(gravityControlMult, controlState.moveZ);
    robo_velocityZ += controlState.moveZ * (float)deltaTime * airSpeedCof * 0.1f;

    if(bounce()==2)
    {
        bounces++;
    }
    robot->core->x += robo_velocityX * (float)deltaTime;
    robot->core->y += robo_velocityY * (float)deltaTime;
    robot->core->z += controlState.moveZ * 5.0f * (float)deltaTime;
    robot->core->z += robo_velocityZ * (float)deltaTime;
}


// Movement when riding rocket. Uses angle math to determine momentum based on robot-core orientation
static void move_robot_riding(double deltaTime)
{
    robot->core->rotZ -= controlState.moveX * 3.0f * (float)deltaTime * rocketAngleCof;
    robot->core->rotZ = fmin(fmax(robot->core->rotZ,-70.0f), 70.0f);


    robot->core->rotY -= controlState.moveX * (float)deltaTime * rocketAngleCof;
    robot->core->rotX -= controlState.moveZ * (float)deltaTime * rocketAngleCof;

    robot->core->rotY = fminf(fmaxf(robot->core->rotY, -20.0f), 20.0f); // Clamp yaw to avoid extreme angles
    robot->core->rotX = fminf(fmaxf(robot->core->rotX, -20.0f), 20.0f); // Clamp pitch to avoid extreme angles

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

    rocketRideTimer += (float)deltaTime;

    //if (controlState.jump || bounce() || rocketRideTimer > 3.0f)
    //Disabled timer for example showing
    if (controlState.jump || bounce())
    {
        isInAir = 1;
        let_go_rocket(robot);
        rockets_remove(ride_rocket);
        bounces=0;
        rocketRideTimer = 0.0f;
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
        rockets_spawn(randomFloat(-80, 80), randomFloat(30, 250), robot->core->z-randomFloat(rocketSpawnDistance, 990), 0.0f, 0.0f, -rocketSpeedCof);
    }
    rockets_update(deltaTime, robot->core->z + 30.0f);
}

void updateEngine(double deltaTime)
{
    //Update each of our units to go along with the frame time
    robot_update(robot, deltaTime); //Function in Robot.c
    update_all_rockets(deltaTime); //Function in rockets.c
    particles_update(deltaTime); //Function in particles.c

    //If not ride rocket, set robot to in air and try to find rocket to ride
    if (!ride_rocket)
    {
        isInAir = 1;
        ride_rocket = rockets_findCollision(robot->core->x, robot->core->y, robot->core->z, 2.0f);
    }
    //If ride rocket found, try to grab it. This effectively adds the rocket Shell as a joint child of the robot core
    if(ride_rocket)
    {
        if(!ride_rocket->shell->parent){
            grab_rocket(robot, ride_rocket);
            score++;
            rocketSpeedCof = baseRocketSpeedCof + score * 1.1f;
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
    if (controlState.restart == 1.0f || bounces == 2){
        restart();
    }
    animate_robot(robot, (float)deltaTime);
    camera_update(robot->core, robo_velocityX, robo_velocityZ, robo_velocityY, deltaTime);
}


//Simple getters for score and bounce count
int engine_getBounceCount(void)
{
    return bounces;
}

int engine_getScore(void)
{
    return score;
}
