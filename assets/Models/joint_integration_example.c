#include "joint.h"
#include "../Shapes/master.h"
#include <GL/glut.h>
#include <math.h>



static Joint* robotBase = NULL;
static Joint* robotArm = NULL;
static Joint* robotHand = NULL;

static float animTime = 0.0f;

void initAnimatedRobot(void) {
    robotBase = joint_create();
    robotArm = joint_create();
    robotHand = joint_create();
    
    robotBase->x = 0.0f;
    robotBase->y = 2.0f;
    robotBase->z = -5.0f;
    
    robotArm->y = 1.5f;
    
    robotHand->y = 2.0f;
    
    joint_addChild(robotBase, robotArm);
    joint_addChild(robotArm, robotHand);
}


void updateAnimatedRobot(float deltaTime) {
    animTime += deltaTime;
    
    robotBase->rotY = animTime * 20.0f;
    
    robotArm->rotZ = sin(animTime * 2.0f) * 45.0f;
    
    robotHand->rotX = sin(animTime * 5.0f) * 20.0f;
}

void drawAnimatedRobot(float deltaTime) {
    updateAnimatedRobot(deltaTime);
    
    float robotColor[4] = {0.7f, 0.7f, 0.9f, 1.0f};
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, robotColor);
    
    /* Draw base (sphere) */
    glPushMatrix();
        joint_applyTransform(robotBase);
        drawSphere(0.8f);
    glPopMatrix();
    
    /* Draw arm (box) */
    glPushMatrix();
        joint_applyTransform(robotArm);
        drawBox(0.5f, 2.0f, 0.5f);
    glPopMatrix();
    
    /* Draw hand (smaller sphere) */
    glPushMatrix();
        joint_applyTransform(robotHand);
        drawSphere(0.4f);
    glPopMatrix();
}