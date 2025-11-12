#include "Robot.h"
#include "../Shapes/master.h"
#include <GL/glut.h>
#include <stdlib.h>
#include <math.h>

// Create a new robot instance
Robot *robot_create(void)
{
    Robot *robot = (Robot *)malloc(sizeof(Robot));
    if (!robot)
        return NULL;

    robot->torso = NULL;
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

    return robot;
}

// Initialize robot joints and hierarchy
void robot_init(Robot *robot, float x, float y, float z)
{
    if (!robot)
        return;

    // Create joints
    robot->torso = joint_create();
    robot->head = joint_create();
    robot->RUpperArm = joint_create();
    robot->RForearm = joint_create();
    robot->LUpperArm = joint_create();
    robot->LForearm = joint_create();
    robot->RThigh = joint_create();
    robot->RCalf = joint_create();
    robot->LThigh = joint_create();
    robot->LCalf = joint_create();

    // Position torso in world
    robot->torso->x = x;
    robot->torso->y = y;
    robot->torso->z = z;
    robot->torso->animatingRot = 0;

    // Head on top of torso
    robot->head->x = 0.0f;
    robot->head->y = 0.8f; // top of torso (torso height is 1.2, so 1.2/2 = 0.6 + margin)
    robot->head->z = 0.0f;

    // Right upper arm (moved closer to match skinnier torso)
    robot->RUpperArm->x = 0.4f;
    robot->RUpperArm->y = 0.5f;
    robot->RUpperArm->z = 0.0f;

    // Right forearm
    robot->RForearm->y = -0.5f;

    // Left upper arm (moved closer to match skinnier torso)
    robot->LUpperArm->x = -0.4f;
    robot->LUpperArm->y = 0.5f;
    robot->LUpperArm->z = 0.0f;

    // Left forearm
    robot->LForearm->y = -0.5f;

    // Right thigh
    robot->RThigh->x = 0.3f;
    robot->RThigh->y = -0.6f;
    robot->RThigh->z = 0.0f;

    // Right calf
    robot->RCalf->y = -0.6f;

    // Left thigh
    robot->LThigh->x = -0.3f; // left side
    robot->LThigh->y = -0.6f; // below torso
    robot->LThigh->z = 0.0f;

    // Left calf
    robot->LCalf->y = -0.6f; // calf below knee

    // Build hierarchy: torso -> head and limbs; upper limbs -> lower limbs
    joint_addChild(robot->torso, robot->head);
    joint_addChild(robot->torso, robot->RUpperArm);
    joint_addChild(robot->RUpperArm, robot->RForearm);
    joint_addChild(robot->torso, robot->LUpperArm);
    joint_addChild(robot->LUpperArm, robot->LForearm);
    joint_addChild(robot->torso, robot->RThigh);
    joint_addChild(robot->RThigh, robot->RCalf);
    joint_addChild(robot->torso, robot->LThigh);
    joint_addChild(robot->LThigh, robot->LCalf);
}

// Update robot animation
void robot_update(Robot *robot, float deltaTime)
{
    if (!robot)
        return;

    // Update all joint animations
    joint_updateAnimation(robot->torso, deltaTime);
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
                     float torsoX, float torsoY, float torsoZ,
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

    // Animate torso rotation
    joint_animateToAngle(robot->torso, torsoX, torsoY, torsoZ, speed);

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

// In-air animation: random poses with continuous spinning
void robot_inAirAnimation(Robot *robot, float deltaTime, float spinSpeed, float poseDuration)
{
    if (!robot)
        return;

    // Update pose timer and switch poses
    robot->poseTimer += deltaTime;
    if (robot->poseTimer >= poseDuration)
    {
        robot->poseTimer = 0.0f;

        // Generate random pose (you'll need to include the poses header or implement inline)
        extern void robot_randompose(Robot * robot);
        robot_randompose(robot);
    }

    // Continuous torso spinning - directly set rotation
    robot->spinTime += deltaTime;
    robot->torso->rotX = fmod(robot->spinTime * spinSpeed, 360.0f);
}

// Draw the robot
void robot_draw(const Robot *robot)
{
    if (!robot)
        return;

    float robotColor[4] = {0.7f, 0.7f, 0.9f, 1.0f};
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, robotColor);

    /* Draw torso (box) - skinnier width and depth */
    glPushMatrix();
    joint_applyTransform(robot->torso);
    drawBox(0.6f, 1.2f, 0.3f);
    glPopMatrix();

    /* Draw head (sphere) */
    glPushMatrix();
    joint_applyTransform(robot->head);
    drawSphere(0.3f, 16, 16);
    glPopMatrix();

    /* Draw RUpperArm (box with offset to center along length) */
    glPushMatrix();
    joint_applyTransform(robot->RUpperArm);
    glTranslatef(0.0f, -0.25f, 0.0f); // offset half length to draw centered on pivot
    drawBox(0.15f, 0.5f, 0.15f);
    glPopMatrix();

    /* Draw RForearm (box with offset) */
    glPushMatrix();
    joint_applyTransform(robot->RForearm);
    glTranslatef(0.0f, -0.25f, 0.0f);
    drawBox(0.15f, 0.5f, 0.15f);
    glPopMatrix();

    /* Draw LUpperArm (box with offset) */
    glPushMatrix();
    joint_applyTransform(robot->LUpperArm);
    glTranslatef(0.0f, -0.25f, 0.0f);
    drawBox(0.15f, 0.5f, 0.15f);
    glPopMatrix();

    /* Draw LForearm (box with offset) */
    glPushMatrix();
    joint_applyTransform(robot->LForearm);
    glTranslatef(0.0f, -0.25f, 0.0f);
    drawBox(0.15f, 0.5f, 0.15f);
    glPopMatrix();

    /* Draw RThigh (box with offset) */
    glPushMatrix();
    joint_applyTransform(robot->RThigh);
    glTranslatef(0.0f, -0.3f, 0.0f);
    drawBox(0.2f, 0.6f, 0.2f);
    glPopMatrix();

    /* Draw RCalf (box with offset) */
    glPushMatrix();
    joint_applyTransform(robot->RCalf);
    glTranslatef(0.0f, -0.3f, 0.0f);
    drawBox(0.2f, 0.6f, 0.2f);
    glPopMatrix();

    /* Draw LThigh (box with offset) */
    glPushMatrix();
    joint_applyTransform(robot->LThigh);
    glTranslatef(0.0f, -0.3f, 0.0f);
    drawBox(0.2f, 0.6f, 0.2f);
    glPopMatrix();

    /* Draw LCalf (box with offset) */
    glPushMatrix();
    joint_applyTransform(robot->LCalf);
    glTranslatef(0.0f, -0.3f, 0.0f);
    drawBox(0.2f, 0.6f, 0.2f);
    glPopMatrix();
}

// Destroy robot and free resources
void robot_destroy(Robot *robot)
{
    if (!robot)
        return;

    joint_free(robot->torso);
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