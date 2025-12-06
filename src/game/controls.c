#ifdef _WIN32
#include <windows.h>
#include <GL/glut.h>
#elif defined(__APPLE__)
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
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
static int isFullScreen = 0;
static int initWindowX = 50;
static int initWindowY = 50;
static int initWindowW = 800;
static int initWindowH = 600;
static int restartHeld = 0; // latch so restart only triggers on key press edge

/* Global control state accessible to engine */
ControlState controlState = {0};

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
    controlState.jump = 0.0f;
    controlState.camMoveX = 0.0f;
    controlState.camMoveY = 0.0f;
    controlState.camMoveZ = 0.0f;
    controlState.cameraYaw = 0.0f;
    controlState.cameraPitch = 0.0f;
    controlState.restart = 0.0f;

    // Process movement keys
    if ((normalKeys['w'] || normalKeys['W']) && !(normalKeys['s'] || normalKeys['S']))
    {
        controlState.moveZ = 1.0f;
        controlState.camMoveZ = 1.0f;
    }
    else if ((normalKeys['s'] || normalKeys['S']) && !(normalKeys['w'] || normalKeys['W']))
    {
        controlState.moveZ = -1.0f;
        controlState.camMoveZ = -1.0f;
    }

    if ((normalKeys['a'] || normalKeys['A']) && !(normalKeys['d'] || normalKeys['D']))
    {
        controlState.moveX = -1.0f;
        controlState.camMoveX = -1.0f;
    }
    else if ((normalKeys['d'] || normalKeys['D']) && !(normalKeys['a'] || normalKeys['A']))
    {
        controlState.moveX = 1.0f;
        controlState.camMoveX = 1.0f;
    }

    if (normalKeys['f'] || normalKeys['F'])
    {
        if (!isFullScreen)
        {
            // Enter fullscreen; remember current size/pos if not already saved
            initWindowX = glutGet(GLUT_WINDOW_X);
            initWindowY = glutGet(GLUT_WINDOW_Y);
            initWindowW = glutGet(GLUT_WINDOW_WIDTH);
            initWindowH = glutGet(GLUT_WINDOW_HEIGHT);
            glutFullScreen();
            isFullScreen = 1;
        }
        else
        {
            // Leave fullscreen by restoring previous size/position
            glutReshapeWindow(initWindowW, initWindowH);
            glutPositionWindow(initWindowX, initWindowY);
            isFullScreen = 0;
        }
        normalKeys['f'] = normalKeys['F'] = 0; // consume key
    }

    if (normalKeys[' '])
    {
        controlState.jump = 1.0f;
        controlState.camMoveY = 1.0f;
    }
    if (normalKeys['c'] || normalKeys['C'])
        controlState.camMoveY = -1.0f;

    if (normalKeys['o'] || normalKeys['O'])
    {
        controlState.perspectiveToggle = controlState.perspectiveToggle == 0.0f ? 1.0f : 0.0f;
        normalKeys['o'] = normalKeys['O'] = 0; // consume key

    }
    else if (normalKeys['p'] || normalKeys['P'])
    {
        controlState.pause = controlState.pause == 0.0f ? 1.0f : 0.0f;
        normalKeys['p'] = normalKeys['P'] = 0; // consume key
    }

    // Restart on edge of 'r' key
    int restartKeyDown = (normalKeys['r'] || normalKeys['R']);
    if (!restartHeld && restartKeyDown && controlState.restart != 0.5f)
    {
        controlState.restart = 1.0f; // fire on key-down edge only
        restartHeld = 1;
    }
    else if (!restartKeyDown)
    {
        restartHeld = 0; // key released, allow next press to trigger
    }

    /* Handle ESC key to exit */
    if (normalKeys[27])
    {
        glutDestroyWindow(windowId);
        exit(0);
    }

    
    if (specialKeys[GLUT_KEY_LEFT] && !specialKeys[GLUT_KEY_RIGHT])
        controlState.cameraYaw = -1.0f;
    else if (specialKeys[GLUT_KEY_RIGHT] && !specialKeys[GLUT_KEY_LEFT])
        controlState.cameraYaw = 1.0f;

    if (specialKeys[GLUT_KEY_UP] && !specialKeys[GLUT_KEY_DOWN])
        controlState.cameraPitch = 1.0f;
    else if (specialKeys[GLUT_KEY_DOWN] && !specialKeys[GLUT_KEY_UP])
        controlState.cameraPitch = -1.0f;
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

    // Capture initial window metrics for fullscreen toggle restore
    initWindowX = glutGet(GLUT_WINDOW_X);
    initWindowY = glutGet(GLUT_WINDOW_Y);
    initWindowW = glutGet(GLUT_WINDOW_WIDTH);
    initWindowH = glutGet(GLUT_WINDOW_HEIGHT);

    /* Set up GLUT callbacks */
    glutKeyboardFunc(handleKeyboard);
    glutKeyboardUpFunc(handleKeyboardUp);
    glutSpecialFunc(handleSpecialKeys);
    glutSpecialUpFunc(handleSpecialKeysUp);
}
