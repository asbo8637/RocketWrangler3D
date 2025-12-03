#include "Robot.h"
#include "../Shapes/master.h"
#ifdef _WIN32
#include <windows.h>
#include <GL/glut.h>
#elif defined(__APPLE__)
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
#include "./Robot_positions/poses.h"
#include "Rocket.h"
#include "../../src/game/controls.h"
#include "../../src/draw/texture.h"
#include <stdlib.h>
#include <math.h>

static const float poseDuration = 0.2f; // seconds between random in-air poses
static unsigned int leatherTex = 0u;
static unsigned int denimTex = 0u;
static unsigned int flannelTex = 0u;

static void ensureRobotTexturesLoaded(void)
{
    if (leatherTex == 0u)
        leatherTex = loadTexture2D("assets/Textures/coudy-brown-leather-texture-wallpaper-fabric-stock-image-design-5.jpg");
    if (denimTex == 0u)
        denimTex = loadTexture2D("assets/Textures/texture-of-denim-jeans-fabric-background-julien.jpg");
    if (flannelTex == 0u)
        flannelTex = loadTexture2D("assets/Textures/flannel.jpg");
}

static float randomFloat(float min, float max)
{
    float scale = rand() / (float)RAND_MAX; // 0.0 to 1.0
    return min + scale * (max - min);
}

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

    robot->animateStyle = 0;

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
    robot->head->y = 0.7f;
    robot->head->z = 0.0f;

    // Right upper arm
    robot->RUpperArm->x = 0.35f;
    robot->RUpperArm->y = 0.4f; // Near top of upper torso
    robot->RUpperArm->z = 0.0f;

    // Right forearm
    robot->RForearm->y = -0.4f;

    // Left upper arm
    robot->LUpperArm->x = -0.35f;
    robot->LUpperArm->y = 0.4f; // Near top of upper torso
    robot->LUpperArm->z = 0.0f;

    // Left forearm
    robot->LForearm->y = -0.4f;

    // Right thigh
    robot->RThigh->x = 0.14f;
    robot->RThigh->y = -0.25f;
    robot->RThigh->z = 0.0f;

    // Right calf
    robot->RCalf->y = -0.6f;

    // Left thigh
    robot->LThigh->x = -0.14f;
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
static void robot_inAirAnimation(Robot *robot, float deltaTime)
{
    float crunchAngle = fminf(30.0f + controlState.moveZ * 60.0f, 60.0f); // Crunch more when moving up
    float spinSpeed = 550.0f + (250.0f * controlState.moveZ);
    float rollAngle = controlState.moveX * 65.0f;
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
    robot->lowerTorso->rotZ -= rollAngle * deltaTime * 2.0f;

    // Smoothly interpolate core's Y rotation toward target roll angle
    robot->lowerTorso->rotY += -rollAngle * 1.6f * deltaTime;
    robot->lowerTorso->rotY = fminf(fmaxf(robot->lowerTorso->rotY, -45.0f), 45.0f);

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
static void robot_riding_animation(Robot *robot, float deltaTime)
{
    if (!robot)
        return;
    
    //Elevate robot lower torso on top
    robot->lowerTorso->x = 0.0f;
    robot->lowerTorso->y = 1.2f;
    robot->lowerTorso->z = 0.0f;

    robot->lowerTorso->animatingRot = 5;

    // Animate the robot in a riding pose
    robot_ridepose(robot);
}

static void robot_surfing_animation(Robot *robot, float deltaTime)
{
    if (!robot)
        return;

    robot->lowerTorso->x = -0.3f;
    robot->lowerTorso->y = 2.3f;
    robot->lowerTorso->z = 0.0f;

    robot->lowerTorso->rotY = -90.0f;

    robot_surfpose(robot);
}

void animate_robot(Robot *robot, float deltaTime)
{
    if (robot->animateStyle == 0){
        robot_inAirAnimation(robot, deltaTime);
    }
    else if (robot->animateStyle == 1){
        robot_riding_animation(robot, deltaTime);
    }
    else if (robot->animateStyle == 2){
        robot_surfing_animation(robot, deltaTime);
    }
}

// Draw the robot
void robot_draw(const Robot *robot)
{
    if (!robot)
        return;

    ensureRobotTexturesLoaded();

    const float armRadius = 0.12f;
    const float legRadius = 0.15f;

    float generalColor[4] = {0.75f, 0.55f, 0.25f, 1.0f};
    float skinColor[4]  = {0.76f, 0.59f, 0.47f, 1.0f};
    float shoeColor[4]  = {0.08f, 0.08f, 0.08f, 1.0f};
    float brightWhite[4] = {0.9f, 0.9f, 0.9f, 1.0f};
    float brightDenimMod[4] = {1.05f, 1.05f, 1.05f, 1.0f};
    float matteSpec[4]   = {0.04f, 0.04f, 0.04f, 1.0f};
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, matteSpec);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 12.0f);
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, generalColor);

    // Draw lower torso
    glPushMatrix();
    joint_applyTransform(robot->lowerTorso);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, flannelTex);
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, brightWhite);
    glColor3f(0.85f, 0.85f, 0.85f);
    drawBox(0.6f, 0.45f, 0.3f);
    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_TEXTURE_2D);
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, generalColor);
    glPopMatrix();

    // Draw upper torso
    glPushMatrix();
    joint_applyTransform(robot->upperTorso);

        //Draw connecting cylinder
        glTranslatef(0.225f, 0.0f, 0.0f);
        glRotatef(90.0f, 0.0f, 0.0f, 1.0f);
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, flannelTex);
        glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, brightWhite);
        glColor3f(0.9f, 0.9f, 0.9f);
        drawCylinder(0.15f, 0.45f, 18);
        glBindTexture(GL_TEXTURE_2D, 0);
        glDisable(GL_TEXTURE_2D);
        glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, generalColor);
        glRotatef(-90.0f, 0.0f, 0.0f, 1.0f);
        glTranslatef(-0.225f, 0.0f, 0.0f);

    glTranslatef(0.0f, 0.225f, 0.0f);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, flannelTex);
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, brightWhite);
    glColor3f(0.9f, 0.9f, 0.9f);
    drawBox(0.6f, 0.45f, 0.3f);
    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_TEXTURE_2D);
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, generalColor);
    glPopMatrix();

    // Draw head 
    glPushMatrix();
    joint_applyTransform(robot->head);
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, skinColor);
    drawSphere(0.3f, 16, 16);
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, generalColor);

    glTranslatef(0.0f, 0.14f, 0.0f);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, leatherTex);
    glDisable(GL_LIGHTING);
    glColor3f(0.3f, 0.3f, 0.3f);
    drawCowboyHat(0.6f, 1.0f, 0.3f, 20.0f,
                  0.25f, 0.33f,
                  18);
    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_TEXTURE_2D);

    glColor3f(0.0f, 0.0f, 0.0f);
    drawCylinder(0.29f, 0.1f, 16); // Hat Ring
    glEnable(GL_LIGHTING);
    glPopMatrix();

    // Draw RUpperArm
    glPushMatrix();
    joint_applyTransform(robot->RUpperArm);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, flannelTex);
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, brightWhite);
    glColor3f(0.9f, 0.9f, 0.9f);
    drawSphere(armRadius, 16, 16);
    glTranslatef(0.0f, -0.4f, 0.0f); // shorter upper arm
    drawCylinder(armRadius, 0.4f, 18);
    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_TEXTURE_2D);
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, generalColor);
    glPopMatrix();

    // Draw RForearm
    glPushMatrix();
    joint_applyTransform(robot->RForearm);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, flannelTex);
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, brightWhite);
    glColor3f(0.9f, 0.9f, 0.9f);
    drawSphere(armRadius, 16, 16);
    glTranslatef(0.0f, -0.4f, 0.0f);
    drawCylinder(armRadius, 0.4f, 18);

        //hand
        glTranslatef(0.0f, -0.15f, 0.0f);
        glBindTexture(GL_TEXTURE_2D, 0);
        glDisable(GL_TEXTURE_2D);
        glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, skinColor);
        drawSphere(0.15f, 16, 16);
        glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, generalColor);
    glPopMatrix();

    // Draw LUpperArm
    glPushMatrix();
    joint_applyTransform(robot->LUpperArm);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, flannelTex);
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, brightWhite);
    glColor3f(0.9f, 0.9f, 0.9f);
    drawSphere(armRadius, 16, 16);
    glTranslatef(0.0f, -0.4f, 0.0f);
    drawCylinder(armRadius, 0.4f, 18);
    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_TEXTURE_2D);
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, generalColor);
    glPopMatrix();

    // Draw LForearm 
    glPushMatrix();
    joint_applyTransform(robot->LForearm);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, flannelTex);
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, brightWhite);
    glColor3f(0.9f, 0.9f, 0.9f);
    drawSphere(armRadius, 16, 16);
    glTranslatef(0.0f, -0.4f, 0.0f);
    drawCylinder(armRadius, 0.4f, 18);

        //hand
        glTranslatef(0.0f, -0.15f, 0.0f);
        glBindTexture(GL_TEXTURE_2D, 0);
        glDisable(GL_TEXTURE_2D);
        glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, skinColor);
        drawSphere(0.16f, 16, 16);
        glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, generalColor);
    
    glPopMatrix();

    // Draw RThigh
    glPushMatrix();
    joint_applyTransform(robot->RThigh);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, denimTex);
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, brightDenimMod);
    glColor3f(1.0f, 1.0f, 1.0f);
    drawSphere(legRadius, 16, 16);
    glTranslatef(0.0f, -0.5f, 0.0f);
    drawCylinder(legRadius, 0.5f, 18);
    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_TEXTURE_2D);
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, generalColor);
    glPopMatrix();

    // Draw RCalf
    glPushMatrix();
    joint_applyTransform(robot->RCalf);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, denimTex);
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, brightDenimMod);
    glColor3f(1.0f, 1.0f, 1.0f);
    drawSphere(legRadius, 16, 16);
    glTranslatef(0.0f, -0.5f, 0.0f);
    drawCylinder(legRadius, 0.5f, 18);

    //Foot
    glTranslatef(0.0f, -0.13f, -0.11f);
    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_TEXTURE_2D);
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, shoeColor);
    drawBox(0.2f, 0.2f, 0.5f);
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, generalColor);
    glPopMatrix();

    // Draw LThigh
    glPushMatrix();
    joint_applyTransform(robot->LThigh);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, denimTex);
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, brightDenimMod);
    glColor3f(1.0f, 1.0f, 1.0f);
    drawSphere(legRadius, 16, 16);
    glTranslatef(0.0f, -0.5f, 0.0f);
    drawCylinder(legRadius, 0.5f, 18);
    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_TEXTURE_2D);
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, generalColor);
    glPopMatrix();

    // Draw LCalf
    glPushMatrix();
    joint_applyTransform(robot->LCalf);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, denimTex);
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, brightDenimMod);
    glColor3f(1.0f, 1.0f, 1.0f);
    drawSphere(legRadius, 16, 16);
    glTranslatef(0.0f, -0.5f, 0.0f);
    drawCylinder(legRadius, 0.5f, 18);

    //Foot
    glTranslatef(0.0f, -0.13f, -0.11f);
    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_TEXTURE_2D);
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, shoeColor);
    drawBox(0.2f, 0.2f, 0.5f);
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, generalColor);
    glPopMatrix();
}

void let_go_rocket(Robot *robot){
    robot->lowerTorso->x = 0.0f;
    robot->lowerTorso->y = 0.0f;
    robot->lowerTorso->z = 0.0f;

    robot->core->rotX = 0.0f;
    robot->core->rotY = 0.0f;
    robot->core->rotZ = 0.0f;

    robot->lowerTorso->rotX = randomFloat(-180.0f,180.0f);
    robot->lowerTorso->rotY = randomFloat(-20.0f,20.0f);
    robot->lowerTorso->rotZ = randomFloat(-180.0f,180.0f);
    
    robot->lowerTorso->targetRotX = 0.0f;
    robot->lowerTorso->targetRotY = 0.0f;
    robot->lowerTorso->targetRotZ = 0.0f;
    robot->animateStyle = 0;
}

void grab_rocket(Robot *robot, Rocket *rocket)
{
    if (!robot || !rocket)
        return;


    float differenceZ = rocket->shell->z - robot->core->z;
    // Position rocket relative to robot without changing world-space landing spot
    rocket->shell->x = 0.0f;
    rocket->shell->y = -0.2f;
    rocket->shell->z = fminf(fmaxf(differenceZ, -9.2f), -0.8f);

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
