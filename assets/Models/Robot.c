#include "Robot.h"
#include "../Shapes/master.h"
#include <GL/glut.h>
#include "./Robot_positions/poses.h"
#include "Rocket.h"
#include <stdlib.h>
#include <math.h>

// Create a new robot instance
Robot *robot_create(void)
{
    Robot *robot = (Robot *)malloc(sizeof(Robot));
    if (!robot)
        return NULL;

    robot->core = NULL;
    robot->lowerTorso = NULL;
    robot->upperTorso = NULL;
    robot->head = NULL;
    robot->RUpperArm = NULL;
    robot->RForearm = NULL;
    robot->LUpperArm = NULL;
    robot->LForearm = NULL;
    robot->RThigh = NULL;
    robot->RCalf = NULL;
    robot->LThigh = NULL;
    robot->LCalf = NULL;

    // Initialize animation state
    robot->poseTimer = 0.0f;
    robot->spinTime = 0.0f;
    robot->lastCrunchAngle = 1e9f;
    robot->lastRollAngle = 1e9f;

    return robot;
}

// Initialize robot joints and hierarchy
void robot_init(Robot *robot, float x, float y, float z)
{
    if (!robot)
        return;

    // Create joints
    robot->core = joint_create();
    robot->lowerTorso = joint_create();
    robot->upperTorso = joint_create();
    robot->head = joint_create();
    robot->RUpperArm = joint_create();
    robot->RForearm = joint_create();
    robot->LUpperArm = joint_create();
    robot->LForearm = joint_create();
    robot->RThigh = joint_create();
    robot->RCalf = joint_create();
    robot->LThigh = joint_create();
    robot->LCalf = joint_create();

    // Position core joint in world (root of entire robot)
    robot->core->x = x;
    robot->core->y = y;
    robot->core->z = z;

    // Position lower torso (hips/lower back) relative to core
    robot->lowerTorso->x = 0.0f;
    robot->lowerTorso->y = 0.0f;
    robot->lowerTorso->z = 0.0f;
    robot->lowerTorso->angleSpeedCof = 0.3f;

    // Upper torso (chest) - relative to lower torso top, pivot at bottom
    robot->upperTorso->x = 0.0f;
    robot->upperTorso->y = 0.225f;
    robot->upperTorso->z = 0.0f;
    robot->upperTorso->angleSpeedCof = 0.5f;

    // Head on top of upper torso
    robot->head->x = 0.0f;
    robot->head->y = 0.65f;
    robot->head->z = 0.0f;

    // Right upper arm
    robot->RUpperArm->x = 0.4f;
    robot->RUpperArm->y = 0.4f; // Near top of upper torso
    robot->RUpperArm->z = 0.0f;

    // Right forearm
    robot->RForearm->y = -0.5f;

    // Left upper arm
    robot->LUpperArm->x = -0.4f;
    robot->LUpperArm->y = 0.4f; // Near top of upper torso
    robot->LUpperArm->z = 0.0f;

    // Left forearm
    robot->LForearm->y = -0.5f;

    // Right thigh
    robot->RThigh->x = 0.3f;
    robot->RThigh->y = -0.25f;
    robot->RThigh->z = 0.0f;

    // Right calf
    robot->RCalf->y = -0.6f;

    // Left thigh
    robot->LThigh->x = -0.3f;
    robot->LThigh->y = -0.25f;
    robot->LThigh->z = 0.0f;

    // Left calf
    robot->LCalf->y = -0.6f; // calf below knee

    // Build hierarchy: core -> lowerTorso -> upperTorso -> head/arms; lowerTorso -> legs
    joint_addChild(robot->core, robot->lowerTorso);
    joint_addChild(robot->lowerTorso, robot->upperTorso);
    joint_addChild(robot->upperTorso, robot->head);
    joint_addChild(robot->upperTorso, robot->RUpperArm);
    joint_addChild(robot->RUpperArm, robot->RForearm);
    joint_addChild(robot->upperTorso, robot->LUpperArm);
    joint_addChild(robot->LUpperArm, robot->LForearm);
    joint_addChild(robot->lowerTorso, robot->RThigh);
    joint_addChild(robot->RThigh, robot->RCalf);
    joint_addChild(robot->lowerTorso, robot->LThigh);
    joint_addChild(robot->LThigh, robot->LCalf);
}

// Update robot animation
void robot_update(Robot *robot, float deltaTime)
{
    if (!robot)
        return;

    // Update all joint animations
    // Core has no animation
    joint_updateAnimation(robot->lowerTorso, deltaTime);
    joint_updateAnimation(robot->upperTorso, deltaTime);
    joint_updateAnimation(robot->head, deltaTime);
    joint_updateAnimation(robot->RUpperArm, deltaTime);
    joint_updateAnimation(robot->RForearm, deltaTime);
    joint_updateAnimation(robot->LUpperArm, deltaTime);
    joint_updateAnimation(robot->LForearm, deltaTime);
    joint_updateAnimation(robot->RThigh, deltaTime);
    joint_updateAnimation(robot->RCalf, deltaTime);
    joint_updateAnimation(robot->LThigh, deltaTime);
    joint_updateAnimation(robot->LCalf, deltaTime);
}

// Set all joint angles for a stance (animates to target angles)
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
                     float speed)
{
    if (!robot)
        return;

    // Animate lower torso rotation
    joint_animateToAngle(robot->lowerTorso, lowerTorsoX, lowerTorsoY, lowerTorsoZ, speed);

    // Animate upper torso rotation
    joint_animateToAngle(robot->upperTorso, upperTorsoX, upperTorsoY, upperTorsoZ, speed);

    // Animate head rotation
    joint_animateToAngle(robot->head, headX, headY, headZ, speed);

    // Animate right arm rotations
    joint_animateToAngle(robot->RUpperArm, rUpperArmX, rUpperArmY, rUpperArmZ, speed);
    joint_animateToAngle(robot->RForearm, rForearmX, rForearmY, rForearmZ, speed);

    // Animate left arm rotations
    joint_animateToAngle(robot->LUpperArm, lUpperArmX, lUpperArmY, lUpperArmZ, speed);
    joint_animateToAngle(robot->LForearm, lForearmX, lForearmY, lForearmZ, speed);

    // Animate right leg rotations
    joint_animateToAngle(robot->RThigh, rThighX, rThighY, rThighZ, speed);
    joint_animateToAngle(robot->RCalf, rCalfX, rCalfY, rCalfZ, speed);

    // Animate left leg rotations
    joint_animateToAngle(robot->LThigh, lThighX, lThighY, lThighZ, speed);
    joint_animateToAngle(robot->LCalf, lCalfX, lCalfY, lCalfZ, speed);
}

// In-air animation
// Random poses with some sick flips
void robot_inAirAnimation(Robot *robot, float deltaTime, float spinSpeed, float poseDuration, float crunchAngle, float rollAngle)
{
    if (!robot)
        return;

    // Update pose timer and switch poses
    robot->poseTimer += deltaTime;
    if (robot->poseTimer >= poseDuration || crunchAngle != robot->lastCrunchAngle || rollAngle != robot->lastRollAngle)
    {
        robot->poseTimer = 0.0f;

        // Generate random pose with current factors
        robot_randompose(robot, crunchAngle, rollAngle);
        robot->lastCrunchAngle = crunchAngle;
        robot->lastRollAngle = rollAngle;
    }

    // Continuous lower torso spinning - add delta rotation each frame
    robot->lowerTorso->rotX -= spinSpeed * deltaTime;
    if(rollAngle!=0.0f){
        robot->lowerTorso->animatingRot=0;
    }
    else{
        robot->lowerTorso->animatingRot=1;
    }
    robot->lowerTorso->rotZ += rollAngle * deltaTime * 1.0f;

    // Smoothly interpolate core's Y rotation toward target roll angle
    float targetRotY = -rollAngle * 1.5f;
    robot->core->rotY += (targetRotY - robot->core->rotY) * deltaTime * 6.0f;

    // Keep rotation in 0-360 range for cleaner values
    if (robot->lowerTorso->rotX < 0.0f)
        robot->lowerTorso->rotX += 360.0f;
    else if (robot->lowerTorso->rotX >= 360.0f)
        robot->lowerTorso->rotX -= 360.0f;

    if (robot->lowerTorso->rotZ < 0.0f)
        robot->lowerTorso->rotZ += 360.0f;
    else if (robot->lowerTorso->rotZ >= 360.0f)
        robot->lowerTorso->rotZ -= 360.0f;
}

// Riding animation
void robot_riding_animation(Robot *robot, float deltaTime)
{
    if (!robot)
        return;

    // Animate the robot in a riding pose
    robot_ridepose(robot);
}

// Draw the robot
void robot_draw(const Robot *robot)
{
    if (!robot)
        return;

    float robotColor[4] = {0.7f, 0.7f, 0.9f, 1.0f};
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, robotColor);

    // Draw lower torso
    glPushMatrix();
    joint_applyTransform(robot->lowerTorso);
    drawBox(0.6f, 0.45f, 0.3f);
    glPopMatrix();

    // Draw upper torso
    glPushMatrix();
    joint_applyTransform(robot->upperTorso);
    glTranslatef(0.0f, 0.225f, 0.0f);
    drawBox(0.6f, 0.45f, 0.3f);
    glPopMatrix();

    // Draw head (sphere)
    glPushMatrix();
    joint_applyTransform(robot->head);
    drawSphere(0.3f, 16, 16);
    glPopMatrix();

    // Draw RUpperArm (box with offset to center along length)
    glPushMatrix();
    joint_applyTransform(robot->RUpperArm);
    glTranslatef(0.0f, -0.25f, 0.0f); // offset half length to draw centered on pivot
    drawBox(0.15f, 0.5f, 0.15f);
    glPopMatrix();

    // Draw RForearm (box with offset)
    glPushMatrix();
    joint_applyTransform(robot->RForearm);
    glTranslatef(0.0f, -0.25f, 0.0f);
    drawBox(0.15f, 0.5f, 0.15f);
    glPopMatrix();

    // Draw LUpperArm (box with offset)
    glPushMatrix();
    joint_applyTransform(robot->LUpperArm);
    glTranslatef(0.0f, -0.25f, 0.0f);
    drawBox(0.15f, 0.5f, 0.15f);
    glPopMatrix();

    // Draw LForearm (box with offset)
    glPushMatrix();
    joint_applyTransform(robot->LForearm);
    glTranslatef(0.0f, -0.25f, 0.0f);
    drawBox(0.15f, 0.5f, 0.15f);
    glPopMatrix();

    // Draw RThigh (box with offset)
    glPushMatrix();
    joint_applyTransform(robot->RThigh);
    glTranslatef(0.0f, -0.3f, 0.0f);
    drawBox(0.2f, 0.6f, 0.2f);
    glPopMatrix();

    // Draw RCalf (box with offset)
    glPushMatrix();
    joint_applyTransform(robot->RCalf);
    glTranslatef(0.0f, -0.3f, 0.0f);
    drawBox(0.2f, 0.6f, 0.2f);
    glPopMatrix();

    // Draw LThigh (box with offset)
    glPushMatrix();
    joint_applyTransform(robot->LThigh);
    glTranslatef(0.0f, -0.3f, 0.0f);
    drawBox(0.2f, 0.6f, 0.2f);
    glPopMatrix();

    // Draw LCalf (box with offset)
    glPushMatrix();
    joint_applyTransform(robot->LCalf);
    glTranslatef(0.0f, -0.3f, 0.0f);
    drawBox(0.2f, 0.6f, 0.2f);
    glPopMatrix();
}

void grab_rocket(Robot *robot, Rocket *rocket)
{
    if (!robot || !rocket)
        return;

    // Position rocket on his ass
    rocket->shell->x = 0.0f;
    rocket->shell->y = -1.2f;
    rocket->shell->z = -3.0f;

    robot->lowerTorso->rotX = 0.0f;
    robot->lowerTorso->rotY = 0.0f;
    robot->lowerTorso->rotZ = 0.0f;

    robot->core->rotY = 0.0f;
    robot->core->rotX = 0.0f;
    robot->core->rotZ = 0.0f;

    // Make rocket a child of robot
    joint_addChild(robot->core, rocket->shell);
}


// Destroy robot and free resources
void robot_destroy(Robot *robot)
{
    if (!robot)
        return;

    joint_free(robot->core);
    joint_free(robot->lowerTorso);
    joint_free(robot->upperTorso);
    joint_free(robot->head);
    joint_free(robot->RUpperArm);
    joint_free(robot->RForearm);
    joint_free(robot->LUpperArm);
    joint_free(robot->LForearm);
    joint_free(robot->RThigh);
    joint_free(robot->RCalf);
    joint_free(robot->LThigh);
    joint_free(robot->LCalf);
    free(robot);
}