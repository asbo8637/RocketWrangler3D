#ifndef ROBOT_H
#define ROBOT_H

#include "joint_class.h"
#include "Rocket.h"

// Robot "class" structure
typedef struct Robot
{
    Joint *core;       // Root joint for overall orientation
    Joint *lowerTorso; // Lower back/hips
    Joint *upperTorso; // Upper chest
    Joint *head;
    Joint *RUpperArm;
    Joint *RForearm;
    Joint *LUpperArm;
    Joint *LForearm;
    Joint *RThigh;
    Joint *RCalf;
    Joint *LThigh;
    Joint *LCalf;

    // Animation state for in-air animation
    float poseTimer;
    float spinTime;
    float lastCrunchAngle;
    float lastRollAngle;

    int animateStyle;
} Robot;

// Create a new robot instance
Robot *robot_create(void);

// Initialize robot joints and hierarchy
void robot_init(Robot *robot, float x, float y, float z);

// Update robot animation
void robot_update(Robot *robot, float deltaTime);

// Draw the robot
void robot_draw(const Robot *robot);

// Set all joint angles for a stance (animates to target angles)
// Pass angles in degrees for each joint (rotX, rotY, rotZ)
// All joints animate at 90 degrees/second
void robot_setStance(Robot *robot,
                     float lowerTorsoX, float lowerTorsoY, float lowerTorsoZ,
                     float upperTorsoX, float upperTorsoY, float upperTorsoZ,
                     float headX, float headY, float headZ,
                     float rUpperArmX, float rUpperArmY, float rUpperArmZ,
                     float rForearmX, float rForearmY, float rForearmZ,
                     float lUpperArmX, float lUpperArmY, float lUpperArmZ,
                     float lForearmX, float lForearmY, float lForearmZ,
                     float rThighX, float rThighY, float rThighZ,
                     float rCalfX, float rCalfY, float rCalfZ,
                     float lThighX, float lThighY, float lThighZ,
                     float lCalfX, float lCalfY, float lCalfZ,
                     float speed);

// In-air animation: random poses with continuous spinning
// spinSpeed: degrees per second for torso rotation
// poseDuration: seconds between random pose changes
void robot_inAirAnimation(Robot *robot, float deltaTime);

// Riding animations
void robot_riding_animation(Robot *robot, float deltaTime);
void surfing_ride_animation(Robot *robot, float deltaTime);

void animate_robot(Robot *robot, float deltaTime);

void grab_rocket(Robot *robot, Rocket *rocket);
void let_go_rocket(Robot *robot);

// Destroy robot and free resources
void robot_destroy(Robot *robot);

#endif // ROBOT_H