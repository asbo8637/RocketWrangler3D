#include <GL/glut.h>
#include <stdlib.h>
#include <string.h>
#include "../draw/camera.h"
#include "controls.h"

#define MAX_KEYS 256
#define MAX_SPECIAL_KEYS 256

/* Key state tracking */
static unsigned char normalKeys[MAX_KEYS] = {0};
static unsigned char specialKeys[MAX_SPECIAL_KEYS] = {0};

/* Other global state */
static int windowId = 0;

/* Global control state accessible to engine */
ControlState controlState = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f};

void handleKeyboard(unsigned char key, int x, int y)
{
    (void)x;
    (void)y;
    normalKeys[key] = 1;
}

void handleKeyboardUp(unsigned char key, int x, int y)
{
    (void)x;
    (void)y;
    normalKeys[key] = 0;
}

void handleSpecialKeys(int key, int x, int y)
{
    (void)x;
    (void)y;
    if (key < MAX_SPECIAL_KEYS)
        specialKeys[key] = 1;
}

void handleSpecialKeysUp(int key, int x, int y)
{
    (void)x;
    (void)y;
    if (key < MAX_SPECIAL_KEYS)
        specialKeys[key] = 0;
}

void processInputs(double deltaTime)
{
    // Reset controls
    controlState.moveX = 0.0f;
    controlState.moveY = 0.0f;
    controlState.moveZ = 0.0f;
    controlState.cameraYaw = 0.0f;
    controlState.cameraPitch = 0.0f;
    controlState.cameraZoom = 0.0f;
    controlState.jump = 0.0f;
    controlState.restart = 0.0f;

    // Process movement keys
    if ((normalKeys['w'] || normalKeys['W']) && !(normalKeys['s'] || normalKeys['S']))
    {
        controlState.moveZ = 1.0f;
    }
    else if ((normalKeys['s'] || normalKeys['S']) && !(normalKeys['w'] || normalKeys['W']))
    {
        controlState.moveZ = -1.0f;
    }

    if ((normalKeys['a'] || normalKeys['A']) && !(normalKeys['d'] || normalKeys['D']))
    {
        controlState.moveX = -1.0f;
    }
    else if ((normalKeys['d'] || normalKeys['D']) && !(normalKeys['a'] || normalKeys['A']))
    {
        controlState.moveX = 1.0f;
    }

    if( normalKeys['f'] || normalKeys['F']){
        glutFullScreen();
    }

    /* Zoom */
    if (normalKeys['+'] || normalKeys['='])
    {
        zoomIn(deltaTime);
        controlState.cameraZoom = 1.0f;
    }
    else if (normalKeys['-'] || normalKeys['_'])
    {
        zoomOut(deltaTime);
        controlState.cameraZoom = -1.0f;
    }

    if (normalKeys[' '])
    {
        controlState.jump = 1.0f;
    }

    if( normalKeys['r'] || normalKeys['R']){
        controlState.restart = 1.0f;
    }

    /* Camera rotation */
    if (specialKeys[GLUT_KEY_LEFT])
    {
        turnLeft(deltaTime);
        controlState.cameraYaw = -1.0f;
    }
    if (specialKeys[GLUT_KEY_RIGHT])
    {
        turnRight(deltaTime);
        controlState.cameraYaw = 1.0f;
    }
    if (specialKeys[GLUT_KEY_UP])
    {
        lookUp(deltaTime);
        controlState.cameraPitch = 1.0f;
    }
    if (specialKeys[GLUT_KEY_DOWN])
    {
        lookDown(deltaTime);
        controlState.cameraPitch = -1.0f;
    }

    /* Handle ESC key to exit */
    if (normalKeys[27])
    {
        glutDestroyWindow(windowId);
        exit(0);
    }
}

int isKeyPressed(unsigned char key)
{
    return normalKeys[key];
}

int isSpecialKeyPressed(int key)
{
    return (key < MAX_SPECIAL_KEYS) ? specialKeys[key] : 0;
}

void initControls(int winId)
{
    windowId = winId;

    /* Reset all key states */
    memset(normalKeys, 0, sizeof(normalKeys));
    memset(specialKeys, 0, sizeof(specialKeys));

    /* Set up GLUT callbacks */
    glutKeyboardFunc(handleKeyboard);
    glutKeyboardUpFunc(handleKeyboardUp);
    glutSpecialFunc(handleSpecialKeys);
    glutSpecialUpFunc(handleSpecialKeysUp);
}